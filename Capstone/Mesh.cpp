#include "Mesh.h"

Mesh::Mesh(void)
{
	pVB = NULL;
	pIB = NULL;
	pTexture = NULL;
	pBump = NULL;
	mNumFaces = mNumVerts = 0;
}

Mesh::~Mesh(void)
{
	SAFE_RELEASE(pVB);
	SAFE_RELEASE(pIB);
	SAFE_RELEASE(pTexture);
	SAFE_RELEASE(pBump);
}

void Mesh::Render(ID3DXEffect* effect)
{
	if(FAILED(effect->SetTexture("ModelTexture", pTexture)))
		LogManager::getInstance()->trace("[ERROR] SetTexture() failed.");
	if(pBump == NULL)
		effect->SetTechnique(effect->GetTechniqueByName("NoBump"));
	else
	{
		effect->SetTechnique(effect->GetTechniqueByName("BumpMap"));
		if(FAILED(effect->SetTexture("NormalMap", pBump)))
			LogManager::getInstance()->trace("[ERROR] SetNormalMap() failed.");
	}
	//effect->SetTechnique(effect->GetTechniqueByName("Debug"));
	UINT cPasses;
	//run the shader code
	effect->Begin(&cPasses, 0);
	for(UINT j = 0; j < cPasses; j++)
	{
		effect->BeginPass(j);
		DirectXRenderer::getInstance()->setGameVertDecl();
		DirectXRenderer::getInstance()->getDevice()->SetStreamSource(0, pVB, 0, sizeof(DirectXRenderer::VERTEX3D));
		DirectXRenderer::getInstance()->getDevice()->SetIndices(pIB);
		DirectXRenderer::getInstance()->getDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mNumVerts, 0, mNumFaces);
		effect->EndPass();
	}
	effect->SetTexture("ModelTexture", NULL);
	effect->SetTexture("NormalMap", NULL);
	effect->End();
}

void Mesh::loadTexture(aiScene* scene, UINT mesh)
{
	//for(UINT j = 0; j < scene->mNumMeshes; ++j)
	//{
		aiString szPath;
		const aiMaterial* pcMat = scene->mMaterials[scene->mMeshes[mesh]->mMaterialIndex];
		if(AI_SUCCESS == aiGetMaterialString(pcMat, AI_MATKEY_TEXTURE_DIFFUSE(0),&szPath))
		{
			if('*' == szPath.data[0])
			{
				UINT index = atoi(szPath.data+1);
				if(index < scene->mNumTextures)
				{
					if(0 == scene->mTextures[index]->mHeight)
					{
						D3DXIMAGE_INFO info;
						if(FAILED(D3DXCreateTextureFromFileInMemoryEx(DirectXRenderer::getInstance()->getDevice(),
							scene->mTextures[index]->pcData,
							scene->mTextures[index]->mWidth,
							D3DX_DEFAULT,
							D3DX_DEFAULT,
							1,
							D3DUSAGE_AUTOGENMIPMAP,
							D3DFMT_UNKNOWN,
							D3DPOOL_MANAGED,
							D3DX_DEFAULT,
							D3DX_DEFAULT,
							NULL,
							&info,
							NULL,
							&pTexture)))
						{
							LogManager::getInstance()->trace("[ERROR] Unable to load the embedded texture (1): %s", szPath.data);
							//continue;
						}
					}
					else
					{
						if(FAILED(DirectXRenderer::getInstance()->getDevice()->CreateTexture(
							scene->mTextures[index]->mWidth,
							scene->mTextures[index]->mHeight,
							NULL,D3DUSAGE_AUTOGENMIPMAP,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&pTexture,NULL)))
						{
							LogManager::getInstance()->trace("[ERROR] Unable to load embedded texture (2): %s", szPath.data);
							//continue;
						}

						D3DLOCKED_RECT sLock;
						pTexture->LockRect(NULL,&sLock,NULL,NULL);

						const aiTexel* pcData = scene->mTextures[index]->pcData;

						for(UINT y = 0; y < scene->mTextures[index]->mHeight; ++y)
						{
							memcpy(sLock.pBits, pcData, scene->mTextures[index]->mWidth * sizeof(aiTexel));
							sLock.pBits = (char*)sLock.pBits + sLock.Pitch;
							pcData += scene->mTextures[index]->mWidth;
						}
						pTexture->UnlockRect(NULL);
						pTexture->GenerateMipSubLevels();
					}
				}
				else
				{
					LogManager::getInstance()->trace("[ERROR] Invalid index for embedded texture: %s", szPath.data);
					//continue;
				}
			}
			else
			{
				char path[255];
				sprintf_s(path, "Resources/Textures/%s", szPath.data);

				if(FAILED(D3DXCreateTextureFromFileEx(DirectXRenderer::getInstance()->getDevice(),
					path,
					D3DX_DEFAULT,
					D3DX_DEFAULT,
					NULL,
					NULL,
					D3DFMT_A8R8G8B8,
					D3DPOOL_MANAGED,
					D3DX_DEFAULT,
					D3DX_DEFAULT,
					NULL,
					NULL,
					NULL,
					&pTexture)))
				{
					LogManager::getInstance()->trace("[ERROR] Unable to load texture: %s", szPath.data);
					//continue;
				}
			}
		}
		if(AI_SUCCESS == aiGetMaterialString(pcMat, AI_MATKEY_TEXTURE_NORMALS(0), &szPath))
		{
			if('*' == szPath.data[0])
			{
				UINT index = atoi(szPath.data+1);
				if(index < scene->mNumTextures)
				{
					if(0 == scene->mTextures[index]->mHeight)
					{
						D3DXIMAGE_INFO info;
						if(FAILED(D3DXCreateTextureFromFileInMemoryEx(DirectXRenderer::getInstance()->getDevice(),
							scene->mTextures[index]->pcData,
							scene->mTextures[index]->mWidth,
							D3DX_DEFAULT,
							D3DX_DEFAULT,
							1,
							D3DUSAGE_AUTOGENMIPMAP,
							D3DFMT_UNKNOWN,
							D3DPOOL_MANAGED,
							D3DX_DEFAULT,
							D3DX_DEFAULT,
							NULL,
							&info,
							NULL,
							&pBump)))
						{
							LogManager::getInstance()->trace("[ERROR] Unable to load the embedded texture (1): %s", szPath.data);
							//continue;
						}
						//else
							//textures.push_back(temp);
					}
					else
					{
						if(FAILED(DirectXRenderer::getInstance()->getDevice()->CreateTexture(
							scene->mTextures[index]->mWidth,
							scene->mTextures[index]->mHeight,
							NULL,D3DUSAGE_AUTOGENMIPMAP,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&pBump,NULL)))
						{
							LogManager::getInstance()->trace("[ERROR] Unable to load embedded texture (2): %s", szPath.data);
							//continue;
						}

						D3DLOCKED_RECT sLock;
						pBump->LockRect(NULL,&sLock,NULL,NULL);

						const aiTexel* pcData = scene->mTextures[index]->pcData;

						for(UINT y = 0; y < scene->mTextures[index]->mHeight; ++y)
						{
							memcpy(sLock.pBits, pcData, scene->mTextures[index]->mWidth * sizeof(aiTexel));
							sLock.pBits = (char*)sLock.pBits + sLock.Pitch;
							pcData += scene->mTextures[index]->mWidth;
						}
						pBump->UnlockRect(NULL);
						pBump->GenerateMipSubLevels();
					}
				}
				else
				{
					LogManager::getInstance()->trace("[ERROR] Invalid index for embedded texture: %s", szPath.data);
					//continue;
				}
			}
			else
			{
				char path[255];
				sprintf_s(path, "Resources/Textures/%s", szPath.data);

				if(FAILED(D3DXCreateTextureFromFileEx(DirectXRenderer::getInstance()->getDevice(),
					path,
					D3DX_DEFAULT,
					D3DX_DEFAULT,
					NULL,
					NULL,
					D3DFMT_A8R8G8B8,
					D3DPOOL_MANAGED,
					D3DX_DEFAULT,
					D3DX_DEFAULT,
					NULL,
					NULL,
					NULL,
					&pBump)))
				{
					LogManager::getInstance()->trace("[ERROR] Unable to load texture: %s", szPath.data);
					//continue;
				}
			}
		}
		else if(AI_SUCCESS == aiGetMaterialString(pcMat, AI_MATKEY_TEXTURE_HEIGHT(0), &szPath))
		{
			if('*' == szPath.data[0])
			{
				UINT index = atoi(szPath.data+1);
				if(index < scene->mNumTextures)
				{
					if(0 == scene->mTextures[index]->mHeight)
					{
						D3DXIMAGE_INFO info;
						if(FAILED(D3DXCreateTextureFromFileInMemoryEx(DirectXRenderer::getInstance()->getDevice(),
							scene->mTextures[index]->pcData,
							scene->mTextures[index]->mWidth,
							D3DX_DEFAULT,
							D3DX_DEFAULT,
							1,
							D3DUSAGE_AUTOGENMIPMAP,
							D3DFMT_UNKNOWN,
							D3DPOOL_MANAGED,
							D3DX_DEFAULT,
							D3DX_DEFAULT,
							NULL,
							&info,
							NULL,
							&pBump)))
						{
							LogManager::getInstance()->trace("[ERROR] Unable to load the embedded texture (1): %s", szPath.data);
							//continue;
						}
						//else
							//textures.push_back(temp);
					}
					else
					{
						if(FAILED(DirectXRenderer::getInstance()->getDevice()->CreateTexture(
							scene->mTextures[index]->mWidth,
							scene->mTextures[index]->mHeight,
							NULL,D3DUSAGE_AUTOGENMIPMAP,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&pBump,NULL)))
						{
							LogManager::getInstance()->trace("[ERROR] Unable to load embedded texture (2): %s", szPath.data);
							//continue;
						}

						D3DLOCKED_RECT sLock;
						pBump->LockRect(NULL,&sLock,NULL,NULL);

						const aiTexel* pcData = scene->mTextures[index]->pcData;

						for(UINT y = 0; y < scene->mTextures[index]->mHeight; ++y)
						{
							memcpy(sLock.pBits, pcData, scene->mTextures[index]->mWidth * sizeof(aiTexel));
							sLock.pBits = (char*)sLock.pBits + sLock.Pitch;
							pcData += scene->mTextures[index]->mWidth;
						}
						pBump->UnlockRect(NULL);
						pBump->GenerateMipSubLevels();
					}
				}
				else
				{
					LogManager::getInstance()->trace("[ERROR] Invalid index for embedded texture: %s", szPath.data);
					//continue;
				}
			}
			else
			{
				char path[255];
				sprintf_s(path, "Resources/Textures/%s", szPath.data);

				if(FAILED(D3DXCreateTextureFromFileEx(DirectXRenderer::getInstance()->getDevice(),
					path,
					D3DX_DEFAULT,
					D3DX_DEFAULT,
					NULL,
					NULL,
					D3DFMT_A8R8G8B8,
					D3DPOOL_MANAGED,
					D3DX_DEFAULT,
					D3DX_DEFAULT,
					NULL,
					NULL,
					NULL,
					&pBump)))
				{
					LogManager::getInstance()->trace("[ERROR] Unable to load texture: %s", szPath.data);
					//continue;
				}
			}
		}
	//}
}