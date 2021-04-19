#include "DebugCube.h"
#include "Colour.h"

LPDIRECT3DVERTEXBUFFER9 DebugCube::pVB = NULL;
LPDIRECT3DTEXTURE9		DebugCube::pDefault = NULL;

DebugCube::DebugCube(Vector position, float width, float length, float height, DebugCube::MOVEMENT_TYPE type, float mass)
{
	vecPosition = position;
	vecScale.SetValues(width, height, length);
	if(type == STATIC)
		bPortal = true;
	else if(type == DYNAMIC)
		bPortal = false;

	pTexture = NULL;
	pBump = NULL;

	fU = fV = 1.f;

	DirectXRenderer::getInstance()->getHKWorld()->lock();
	{
		hkVector4 half;
		half.set((hkReal)width/2.f, (hkReal)height/2.f, (hkReal)length/2.f);
		hkpBoxShape* box = new hkpBoxShape(half,0);

		const hkReal boxMass = mass;
		hkMassProperties massProperties;
		hkpInertiaTensorComputer::computeShapeVolumeMassProperties(box, boxMass, massProperties);

		hkpRigidBodyCinfo bodyInfo;

		bodyInfo.setMassProperties(massProperties);
		if(type == DYNAMIC)
			bodyInfo.m_motionType = hkpMotion::MOTION_DYNAMIC;
		else if(type == STATIC)
			bodyInfo.m_motionType = hkpMotion::MOTION_FIXED;
		else
			bodyInfo.m_motionType = hkpMotion::MOTION_FIXED;
		bodyInfo.m_shape = box;
		bodyInfo.m_position.set(vecPosition.GetX(), vecPosition.GetY(), vecPosition.GetZ());
		//bodyInfo.m_contactPointCallbackDelay = 0;

		bodyInfo.m_friction = 1.f;
		bodyInfo.m_restitution = 0.f;

		pBody = new hkpRigidBody(bodyInfo);

		DirectXRenderer::getInstance()->getHKWorld()->addEntity(pBody);

		pBody->setUserData(RenderablePhysicsMap::getInstance()->getIndex());
		RenderablePhysicsMap::getInstance()->theMap[RenderablePhysicsMap::getInstance()->getIndex()] = this;
		RenderablePhysicsMap::getInstance()->incrementIndex();
		
		box->removeReference();
		pBody->removeReference();
	}

	DirectXRenderer::getInstance()->getHKWorld()->unlock();
}

DebugCube::~DebugCube(void)
{
	if(pBody != NULL)
	{
		DirectXRenderer::getInstance()->getHKWorld()->lock();
		DirectXRenderer::getInstance()->getHKWorld()->removeEntity(pBody);
		DirectXRenderer::getInstance()->getHKWorld()->unlock();
		pBody = NULL;
	}
}

void DebugCube::SetUTiling(float u){fU=u;}
void DebugCube::SetVTiling(float v){fV=v;}
void DebugCube::SetUVTiling(float u, float v){fU=u;fV=v;}

void DebugCube::SetTexture(const char* fileName)
{
	char temp[64];
	sprintf_s(temp, "Resources/Textures/%s", fileName);
	if(FAILED(D3DXCreateTextureFromFile(DirectXRenderer::getInstance()->getDevice(), temp, &pTexture)))
		LogManager::getInstance()->trace("[ERROR] Load texture failed (%s)", fileName);
}
void DebugCube::SetBump(const char* fileName)
{
	char temp[64];
	sprintf_s(temp, "Resources/Textures/%s", fileName);
	if(FAILED(D3DXCreateTextureFromFile(DirectXRenderer::getInstance()->getDevice(), temp, &pBump)))
		LogManager::getInstance()->trace("[ERROR] Load texture failed (%s)", fileName);
}

void DebugCube::Render(void)
{
	DirectXRenderer::getInstance()->pushMatrix();
	DirectXRenderer::getInstance()->scale(vecScale);
	DirectXRenderer::getInstance()->rotateAxis(vecRotation, vecRotation.GetW());
	DirectXRenderer::getInstance()->translate(vecPosition);
	DirectXRenderer::getInstance()->updateLighting();
	if(userData != CYAN)
	{
		ID3DXEffect* effect = DirectXRenderer::getInstance()->getLightingEffect();
		if(pTexture)
		{
			if(FAILED(effect->SetTexture("ModelTexture", pTexture)))
				LogManager::getInstance()->trace("[ERROR] SetTexture(CUBE) failed.");
		}
		else
			if(FAILED(effect->SetTexture("ModelTexture", pDefault)))
				LogManager::getInstance()->trace("[ERROR] SetTexture(CUBE, Default) failed.");
		if(pBump)
		{
			if(FAILED(effect->SetTexture("NormalMap", pBump)))
				LogManager::getInstance()->trace("[ERROR] SetNormalMap(CUBE) failed.");
			effect->SetTechnique(effect->GetTechniqueByName("BumpMap"));
		}
		else
		{
			effect->SetTechnique(effect->GetTechniqueByName("NoBump"));
		}
		effect->SetVector("TextureTiling", &D3DXVECTOR4(fU, fV, 0, 0));
		UINT cPasses;
		//run the shader code
		effect->Begin(&cPasses, 0);
		for(UINT j = 0; j < cPasses; j++)
		{
			effect->BeginPass(j);
			//RENDER!
			DirectXRenderer::getInstance()->setGameVertDecl();
			DirectXRenderer::getInstance()->getDevice()->SetStreamSource(0, pVB, 0, sizeof(DirectXRenderer::VERTEX3D));
			for(UINT i = 0; i < 6; ++i)
				DirectXRenderer::getInstance()->getDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, i*4, 2);
			effect->EndPass();
		}
		effect->SetVector("TextureTiling", &D3DXVECTOR4(1.f, 1.f, 0, 0));
		effect->SetTexture("ModelTexture", NULL);
		effect->SetTexture("NormalMap", NULL);
		effect->End();
	}
	else
	{
		DirectXRenderer::getInstance()->setGameVertDecl();
		DirectXRenderer::getInstance()->getDevice()->SetTexture(0, pTexture);
		DirectXRenderer::getInstance()->getDevice()->SetStreamSource(0, pVB, 0, sizeof(DirectXRenderer::VERTEX3D));
		for(UINT i = 0; i < 6; ++i)
			DirectXRenderer::getInstance()->getDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, i*4, 2);
	}
	DirectXRenderer::getInstance()->popMatrix();
}

void DebugCube::Update(double dTime)
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