#include "SceneNodePhysics.h"
#include "assimp\include\assimp.h"
#include "assimp\include\aiScene.h"
#include "assimp\include\aiPostProcess.h"

SceneNodePhysics::SceneNodePhysics(const char* fileName, float x, float y, float z, SCENENODE_TYPE type, float mass)
{
	vecPosition.SetValues(x,y,z);
	vecRotation.SetValues(0,0,0);
	vecOffset.SetValues(0,0,0);
	vecMin.SetValues(0,0,0);
	vecMax.SetValues(0,0,0);
	fScale = 1.f;
	mNumMeshes = 0;
	char temp[255];
	sprintf_s(temp, "Resources/Models/%s", fileName);
	if(loadAsset(temp))
	{
		LogManager::getInstance()->trace("[ERROR] Could not initiate Physical Scene Node (%s)", fileName);
		delete this;
		return;
	}
	createRigidBody(type, mass);
}

SceneNodePhysics::~SceneNodePhysics(void)
{
	if(pBody != NULL)
	{
		DirectXRenderer::getInstance()->getHKWorld()->lock();
		DirectXRenderer::getInstance()->getHKWorld()->removeEntity(pBody);
		DirectXRenderer::getInstance()->getHKWorld()->unlock();
		pBody = NULL;
	}
}

int SceneNodePhysics::loadAsset(const char* fileName)
{
	aiScene* scene = (aiScene*)aiImportFile(fileName,
		aiProcess_CalcTangentSpace			| //calculate tangents and bitangents, if possible
		aiProcess_JoinIdenticalVertices		| //join identical vertices
		aiProcess_ValidateDataStructure		| //perform a ful validation of the loaders output
		aiProcess_ImproveCacheLocality		| //improve the cache locality of the output vertices
		aiProcess_RemoveRedundantMaterials	| //remove redundant materials
		aiProcess_FindDegenerates			| //remove degenerated polygons from the import
		aiProcess_FindInvalidData			| //detect invalid model data (such as invalid normal vectors and such)
		aiProcess_GenUVCoords				| //convert spherical, cylindrical, box and planar mapping to proper UVs
		aiProcess_TransformUVCoords			| //preprocess UV transformations
		aiProcess_FindInstances				| //search for instanced meshes and remove them by reference to one master
		aiProcess_LimitBoneWeights			| //limit bone weights to 4 per vertex
		aiProcess_OptimizeMeshes			| //if possible, join small meshes
		aiProcess_GenSmoothNormals			| //generate smooth normal vectors
		aiProcess_SplitLargeMeshes			| //split large, unrenderable meshes into submeshes
		aiProcess_Triangulate				| //convert faces to triangles (this is the best feature ever)
		aiProcess_ConvertToLeftHanded		| //make it D3D compatible
		aiProcess_SortByPType				| //make meshes which consist of a single type of primitive
		0);

	if(!scene)
		return 1;

	UINT numVert = 0;
	UINT numTri = 0;

	DirectXRenderer::VERTEX3D* vertices = NULL;
	DWORD* indices = NULL;
	DWORD* attrBuff = NULL;

	mNumMeshes = scene->mNumMeshes;

	for(UINT i = 0; i < mNumMeshes; ++i)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		Mesh* tempMesh = new Mesh();
		//Load Texture
		tempMesh->loadTexture(scene, i);

		tempMesh->mNumVerts = mesh->mNumVertices;
		tempMesh->mNumFaces = mesh->mNumFaces;

		//create VB
		if(FAILED(DirectXRenderer::getInstance()->getDevice()->CreateVertexBuffer(sizeof(DirectXRenderer::VERTEX3D) * mesh->mNumVertices,
			D3DUSAGE_WRITEONLY,
			0,
			D3DPOOL_MANAGED,
			&tempMesh->pVB,
			NULL)))
		{
			LogManager::getInstance()->trace("[ERROR] Failed to create Vertex Buffer (%s)", fileName);
			return 1;
		}

		if(mesh->mNumFaces * 3 >= 65536)
		{
			if(FAILED(DirectXRenderer::getInstance()->getDevice()->CreateIndexBuffer(
				4 * mesh->mNumFaces * 3,
				D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX32,
				D3DPOOL_MANAGED,
				&tempMesh->pIB,
				NULL)))
			{
				LogManager::getInstance()->trace("[ERROR] Failed to create 32-bit Index Buffer (%s)", fileName);
				return 1;
			}

			UINT* piData;
			tempMesh->pIB->Lock(NULL, NULL, (void**)&piData, NULL);
			for(UINT j = 0; j < mesh->mNumFaces; ++j)
			{
				for(UINT k = 0; k < 3; ++k)
					*piData++ = mesh->mFaces[j].mIndices[k];
			}
		}
		else
		{
			if(FAILED(DirectXRenderer::getInstance()->getDevice()->CreateIndexBuffer(
				4 * mesh->mNumFaces * 3,
				D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&tempMesh->pIB,
				NULL)))
			{
				LogManager::getInstance()->trace("[ERROR] Failed to create 16-bit Index Buffer (%s)", fileName);
				return 1;
			}

			UINT16* piData;
			tempMesh->pIB->Lock(NULL, NULL, (void**)&piData, NULL);
			for(UINT j = 0; j < mesh->mNumFaces; ++j)
			{
				for(UINT k = 0; k < 3; ++k)
					*piData++ = (UINT16)mesh->mFaces[j].mIndices[k];
			}
		}
		tempMesh->pIB->Unlock();

		DirectXRenderer::VERTEX3D* pvData;
		tempMesh->pVB->Lock(NULL, NULL, (void**)&pvData, NULL);
		for(UINT x = 0; x < mesh->mNumVertices; ++x)
		{
			pvData->pX = mesh->mVertices[x].x;
			pvData->pY = mesh->mVertices[x].y;
			pvData->pZ = mesh->mVertices[x].z;

			//max and mins
			if(vecMax.GetX() < pvData->pX)
				vecMax.SetX(pvData->pX);
			if(vecMin.GetX() > pvData->pX)
				vecMin.SetX(pvData->pX);
			if(vecMax.GetY() < pvData->pY)
				vecMax.SetY(pvData->pY);
			if(vecMin.GetY() > pvData->pY)
				vecMin.SetY(pvData->pY);
			if(vecMax.GetZ() < pvData->pZ)
				vecMax.SetZ(pvData->pZ);
			if(vecMin.GetZ() > pvData->pZ)
				vecMin.SetZ(pvData->pZ);

			if(mesh->mNormals == NULL)
			{
				pvData->nX = pvData->nY = pvData->nZ = 0.0f;
			}
			else
			{
				pvData->nX = mesh->mNormals[x].x;
				pvData->nY = mesh->mNormals[x].y;
				pvData->nZ = mesh->mNormals[x].z;
			}

			if(mesh->mTangents == NULL)
			{
				pvData->tX = pvData->tY = pvData->tZ = 0.0f;
				pvData->bX = pvData->bY = pvData->bZ = 0.0f;
			}
			else
			{
				pvData->tX = mesh->mTangents[x].x;
				pvData->tY = mesh->mTangents[x].y;
				pvData->tZ = mesh->mTangents[x].z;

				pvData->bX = mesh->mBitangents[x].x;
				pvData->bY = mesh->mBitangents[x].y;
				pvData->bZ = mesh->mBitangents[x].z;
			}

			if(mesh->HasTextureCoords(0))
			{
				pvData->u = mesh->mTextureCoords[0][x].x;
				pvData->v = mesh->mTextureCoords[0][x].y;
			}
			else
				pvData->u = pvData->v = 0.5f;

			++pvData;
		}
		tempMesh->pVB->Unlock();

		vpMeshes.push_back(tempMesh);
	}

	return 0;
}

void SceneNodePhysics::createRigidBody(SCENENODE_TYPE type, float mass)
{
	DirectXRenderer::getInstance()->getHKWorld()->lock();
	{
		hkVector4 half;
		half.set((hkReal)((vecMax.GetX()-vecMin.GetX())/2.f), (hkReal)((vecMax.GetY()-vecMin.GetY())/2.f), (hkReal)((vecMax.GetZ()-vecMin.GetZ())/2.f));
		hkpBoxShape* box = new hkpBoxShape(half,0);

		const hkReal boxMass = mass;
		hkMassProperties massProperties;
		hkpInertiaTensorComputer::computeShapeVolumeMassProperties(box, boxMass, massProperties);

		hkpRigidBodyCinfo bodyInfo;

		bodyInfo.m_shape = box;
		bodyInfo.setMassProperties(massProperties);
		if(type == NODE_DYNAMIC)
			bodyInfo.m_motionType = hkpMotion::MOTION_DYNAMIC;
		else if(type == NODE_STATIC)
			bodyInfo.m_motionType = hkpMotion::MOTION_FIXED;
		else
			bodyInfo.m_motionType = hkpMotion::MOTION_FIXED;
		bodyInfo.m_position.set(vecPosition.GetX(), vecPosition.GetY(), vecPosition.GetZ());
		bodyInfo.m_contactPointCallbackDelay = 0;

		bodyInfo.m_friction = 1.f;

		pBody = new hkpRigidBody(bodyInfo);

		pBody->setUserData(RenderablePhysicsMap::getInstance()->getIndex());
		RenderablePhysicsMap::getInstance()->theMap[RenderablePhysicsMap::getInstance()->getIndex()] = this;
		RenderablePhysicsMap::getInstance()->incrementIndex();

		DirectXRenderer::getInstance()->getHKWorld()->addEntity(pBody);

		pBody->setUserData(RenderablePhysicsMap::getInstance()->getIndex());
		RenderablePhysicsMap::getInstance()->theMap[RenderablePhysicsMap::getInstance()->getIndex()] = this;
		RenderablePhysicsMap::getInstance()->incrementIndex();
		
		box->removeReference();
		pBody->removeReference();
	}

	DirectXRenderer::getInstance()->getHKWorld()->unlock();
}

void SceneNodePhysics::Update(double dTime)
{
	if(pBody != NULL)
	{
		DirectXRenderer::getInstance()->getHKWorld()->lock();
		vecPosition.SetValues(pBody->getPosition().m_quad.v[0], pBody->getPosition().m_quad.v[1], pBody->getPosition().m_quad.v[2]);
		if(pBody->getRotation().hasValidAxis())
		{
			hkVector4 temp(0.0, 0.0, 0.0);
			pBody->getRotation().getAxis(temp);
			vecRotation.SetValues(temp.m_quad.v[0], temp.m_quad.v[1], temp.m_quad.v[2], pBody->getRotation().getAngle());
		}
		DirectXRenderer::getInstance()->getHKWorld()->unlock();
	}
}

void SceneNodePhysics::Render(void)
{
	DirectXRenderer::getInstance()->pushMatrix();
	DirectXRenderer::getInstance()->scale(fScale);
	DirectXRenderer::getInstance()->rotateAxis(vecRotation, vecRotation.GetW());
	DirectXRenderer::getInstance()->translate(vecPosition);
	DirectXRenderer::getInstance()->updateLighting();
	ID3DXEffect* effect = DirectXRenderer::getInstance()->getLightingEffect();
	for(UINT i = 0; i < mNumMeshes; ++i)
	{
		vpMeshes[i]->Render(effect);
	}
	DirectXRenderer::getInstance()->popMatrix();
}