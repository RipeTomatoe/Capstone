#include "MaterialManager.h"
#include "Light.h"
#include "Camera.h"

MaterialManager* MaterialManager::pInstance = NULL;
Destroyer<MaterialManager> MaterialManager::destroyer;

static void textFileRead(const char* fileName, char** output)
{
	if(fileName != NULL)
	{
		FILE* file = fopen(fileName, "rt");

		if(file != NULL)
		{
			fseek(file, 0, SEEK_END);
			int count = ftell(file);
			rewind(file);

			if(count > 0)
			{
				(*output) = (char*)malloc(sizeof(char) * (count + 1));
				count = fread((*output), sizeof(char), count, file);
				(*output)[count] = '\0';
			}
			fclose(file);
		}
	}
}

//This creates a texture which is used when the texture could not be loaded!
VOID WINAPI FillFunc(D3DXVECTOR4* pOut,
					 CONST D3DXVECTOR2* pTexCoord,
					 CONST D3DXVECTOR2* pTexelSize,
					 LPVOID pData)
{
	UNREFERENCED_PARAMETER(pData);
	UNREFERENCED_PARAMETER(pTexelSize);

	UINT iX = (UINT)(pTexCoord->x * 256.0f);
	UINT iY = (UINT)(pTexCoord->y * 256.0f);

	bool bGrey = false;
	if((iX / 32) % 2 == 0)
	{
		if((iY / 32) % 2 == 0) bGrey = true;
	}
	else
	{
		if((iY / 32) % 2 == 0) bGrey = true;
	}
	pOut->w = 1.0f;
	if(bGrey)
	{
		pOut->x = pOut->y = pOut->z = 0.3f;
	}
	else
	{
		pOut->x = pOut->y = 1.0f;
		pOut->z = 0.0f;
	}
	return;
}

bool MaterialManager::HasAlphaPixels(IDirect3DTexture9* piTexture)
{
	ai_assert(NULL != piTexture);

	D3DLOCKED_RECT sRect;
	D3DSURFACE_DESC sDesc;
	piTexture->GetLevelDesc(0, &sDesc);
	if(FAILED(piTexture->LockRect(0, &sRect, NULL, D3DLOCK_READONLY)))
		return false;
	const int iPitchDiff = (int)sRect.Pitch - (int)(sDesc.Width * 4);

	struct SColor
	{
		BYTE b,g,r,a;
	};
	const SColor* pcData = (const SColor*)sRect.pBits;

	union
	{
		const SColor* pcPointer;
		const BYTE* pcCharPointer;
	};
	pcPointer = pcData;
	for(UINT y = 0; y < sDesc.Height; ++y)
	{
		for(UINT x = 0; x < sDesc.Width; ++x)
		{
			if(pcPointer->a != 0xff)
			{
				piTexture->UnlockRect(0);
				return true;
			}
			pcPointer++;
		}
		pcCharPointer += iPitchDiff;
	}
	piTexture->UnlockRect(0);
	return false;
}

int MaterialManager::UpdateSpecularMaterials(AssetHelper* pAsset)
{
	if (pAsset && pAsset->paScene)
	{
		for (unsigned int i = 0; i < pAsset->paScene->mNumMeshes;++i)
		{
			if (aiShadingMode_Phong == pAsset->ppaMeshes[i]->eShadingMode)
			{
					this->DeleteMaterial(pAsset->ppaMeshes[i]);
					//this->CreateMaterial(pAsset->ppaMeshes[i],pAsset->paScene->mMeshes[i]);
					//this->CreateMaterial(pAsset,i);
			}
		}
	}
	return 1;
}

int MaterialManager::SetDefaultTexture(IDirect3DTexture9** p_ppiOut)
{
	if(FAILED(DirectXRenderer::getInstance()->getDevice()->CreateTexture(256,256,NULL,NULL,
																		 D3DFMT_A8R8G8B8,
																		 D3DPOOL_MANAGED,
																		 p_ppiOut,
																		 NULL)))
	{
		LogManager::getInstance()->trace("[ERROR] Unable to create the default texture");
		*p_ppiOut = NULL;
	}
	D3DXFillTexture(*p_ppiOut, &FillFunc,NULL);
	return 1;
}

int MaterialManager::LoadTexture(IDirect3DTexture9** p_ppiOut, AssetHelper* pAsset, aiString* szPath)
{
	ai_assert(NULL != p_ppiOut);
	ai_assert(NULL != szPath);

	*p_ppiOut = NULL;

	//'*' means that it is an embedded texture
	if ('*' ==  szPath->data[0])
	{
		UINT index = atoi(szPath->data+1);
		if(index < pAsset->paScene->mNumTextures)
		{
			if(0 == pAsset->paScene->mTextures[index]->mHeight)
			{
				//it's embedded just load the fucker
				D3DXIMAGE_INFO info;
				if(FAILED(D3DXCreateTextureFromFileInMemoryEx(DirectXRenderer::getInstance()->getDevice(),
					pAsset->paScene->mTextures[index]->pcData,
					pAsset->paScene->mTextures[index]->mWidth,
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
					p_ppiOut)))
				{
					LogManager::getInstance()->trace("[ERROR] Unable to load the embedded texture (1): %s", szPath->data);
					this->SetDefaultTexture(p_ppiOut);
					return 1;
				}
			}
			else
			{
				if(FAILED(DirectXRenderer::getInstance()->getDevice()->CreateTexture(
					pAsset->paScene->mTextures[index]->mWidth,
					pAsset->paScene->mTextures[index]->mHeight,
					NULL,D3DUSAGE_AUTOGENMIPMAP,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,p_ppiOut,NULL)))
				{
					LogManager::getInstance()->trace("[ERROR] Unable to load embedded texture (2): %s", szPath->data);
					this->SetDefaultTexture(p_ppiOut);
					return 1;
				}

				D3DLOCKED_RECT sLock;
				(*p_ppiOut)->LockRect(NULL,&sLock,NULL,NULL);

				const aiTexel* pcData = pAsset->paScene->mTextures[index]->pcData;

				for(UINT y = 0; y < pAsset->paScene->mTextures[index]->mHeight; ++y)
				{
					memcpy(sLock.pBits, pcData, pAsset->paScene->mTextures[index]->mWidth * sizeof(aiTexel));
					sLock.pBits = (char*)sLock.pBits + sLock.Pitch;
					pcData += pAsset->paScene->mTextures[index]->mWidth;
				}
				(*p_ppiOut)->UnlockRect(NULL);
				(*p_ppiOut)->GenerateMipSubLevels();
			}
			return 1;
		}
		else
		{
			LogManager::getInstance()->trace("[ERROR] Invalid index for embedded texture: %s", szPath->data);
			this->SetDefaultTexture(p_ppiOut);
			return 1;
		}
	}

	if(FAILED(D3DXCreateTextureFromFileEx(
		DirectXRenderer::getInstance()->getDevice(),
		szPath->data,
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
		p_ppiOut)))
	{
		LogManager::getInstance()->trace("[ERROR] Unable to load texture: %s", szPath->data);
		this->SetDefaultTexture(p_ppiOut);
	}
	return 1;
}

void MaterialManager::DeleteMaterial(AssetHelper::MeshHelper* pcIn)
{
	if(!pcIn || !pcIn->piEffect)return;
	pcIn->piEffect->Release();

	if (pcIn->piDiffuseTexture)
	{
		pcIn->piDiffuseTexture->Release();
		pcIn->piDiffuseTexture = NULL;
	}
	if (pcIn->piSpecularTexture)
	{
		pcIn->piSpecularTexture->Release();
		pcIn->piSpecularTexture = NULL;
	}
	if (pcIn->piEmissiveTexture)
	{
		pcIn->piEmissiveTexture->Release();
		pcIn->piEmissiveTexture = NULL;
	}
	if (pcIn->piAmbientTexture)
	{
		pcIn->piAmbientTexture->Release();
		pcIn->piAmbientTexture = NULL;
	}
	if (pcIn->piOpacityTexture)
	{
		pcIn->piOpacityTexture->Release();
		pcIn->piOpacityTexture = NULL;
	}
	if (pcIn->piNormalTexture)
	{
		pcIn->piNormalTexture->Release();
		pcIn->piNormalTexture = NULL;
	}
	if (pcIn->piShininessTexture)
	{
		pcIn->piShininessTexture->Release();
		pcIn->piShininessTexture = NULL;
	}
	if (pcIn->piLightmapTexture)
	{
		pcIn->piLightmapTexture->Release();
		pcIn->piLightmapTexture = NULL;
	}
	pcIn->piEffect = NULL;
}

int MaterialManager::CreateMaterial(AssetHelper* pAsset, int matIndex)
{
	ID3DXBuffer* piBuffer;

	D3DXMACRO sMacro[64];

	AssetHelper::MeshHelper* pcMesh = pAsset->ppaMeshes[matIndex];
	const aiMesh* pcSource = pAsset->paScene->mMeshes[matIndex];

	const aiMaterial* pcMat = pAsset->paScene->mMaterials[pcSource->mMaterialIndex];

	//Diffuse
	if(AI_SUCCESS != aiGetMaterialColor(pcMat, AI_MATKEY_COLOR_DIFFUSE, (aiColor4D*)&pcMesh->vDiffuseColor))
	{
		pcMesh->vDiffuseColor.x = 1.0f;
		pcMesh->vDiffuseColor.y = 1.0f;
		pcMesh->vDiffuseColor.z = 1.0f;
		pcMesh->vDiffuseColor.w = 1.0f;
	}
	//Specular
	if(AI_SUCCESS != aiGetMaterialColor(pcMat,AI_MATKEY_COLOR_SPECULAR, (aiColor4D*)&pcMesh->vSpecularColor))
	{
		pcMesh->vSpecularColor.x = 1.0f;
		pcMesh->vSpecularColor.y = 1.0f;
		pcMesh->vSpecularColor.z = 1.0f;
		pcMesh->vSpecularColor.w = 1.0f;
	}
	//Ambient
	if(AI_SUCCESS != aiGetMaterialColor(pcMat,AI_MATKEY_COLOR_AMBIENT, (aiColor4D*)&pcMesh->vAmbientColor))
	{
		pcMesh->vAmbientColor.x = 0.0f;
		pcMesh->vAmbientColor.y = 0.0f;
		pcMesh->vAmbientColor.z = 0.0f;
		pcMesh->vAmbientColor.w = 1.0f;
	}
	//Emissive
	if(AI_SUCCESS != aiGetMaterialColor(pcMat,AI_MATKEY_COLOR_EMISSIVE, (aiColor4D*)&pcMesh->vEmissiveColor))
	{
		pcMesh->vEmissiveColor.x = 0.0f;
		pcMesh->vEmissiveColor.y = 0.0f;
		pcMesh->vEmissiveColor.z = 0.0f;
		pcMesh->vEmissiveColor.w = 1.0f;
	}
	//Opacity
	if(AI_SUCCESS != aiGetMaterialFloat(pcMat,AI_MATKEY_OPACITY,&pcMesh->fOpacity))
	{
		pcMesh->fOpacity = 1.0f;
	}
	//Shading Model
	bool bDefault = false;
	if(AI_SUCCESS != aiGetMaterialInteger(pcMat,AI_MATKEY_SHADING_MODEL,(int*)&pcMesh->eShadingMode ))
	{
		bDefault = true;
		pcMesh->eShadingMode = aiShadingMode_Gouraud;
	}
	//Shininess
	if(AI_SUCCESS != aiGetMaterialFloat(pcMat,AI_MATKEY_SHININESS,&pcMesh->fShininess))
	{
		// assume 15 as default shininess
		pcMesh->fShininess = 15.0f;
	}
	else if (bDefault)pcMesh->eShadingMode  = aiShadingMode_Phong;
	//Shininess Strength
	if(AI_SUCCESS != aiGetMaterialFloat(pcMat,AI_MATKEY_SHININESS_STRENGTH,&pcMesh->fSpecularStrength))
	{
		// assume 1.0 as default shininess strength
		pcMesh->fSpecularStrength = 1.0f;
	}

	aiString szPath;

	aiTextureMapMode mapU(aiTextureMapMode_Wrap),mapV(aiTextureMapMode_Wrap);

	bool bib =false;
	if (pcSource->mTextureCoords[0])
	{
		// Diffuse Texture
		if(AI_SUCCESS == aiGetMaterialString(pcMat,AI_MATKEY_TEXTURE_DIFFUSE(0),&szPath))
		{
			LoadTexture(&pcMesh->piDiffuseTexture,pAsset,&szPath);

			aiGetMaterialInteger(pcMat,AI_MATKEY_MAPPINGMODE_U_DIFFUSE(0),(int*)&mapU);
			aiGetMaterialInteger(pcMat,AI_MATKEY_MAPPINGMODE_V_DIFFUSE(0),(int*)&mapV);
		}
		// Specular Texture
		if(AI_SUCCESS == aiGetMaterialString(pcMat,AI_MATKEY_TEXTURE_SPECULAR(0),&szPath))
		{
			LoadTexture(&pcMesh->piSpecularTexture,pAsset,&szPath);
		}
		// Opacity Texture
		if(AI_SUCCESS == aiGetMaterialString(pcMat,AI_MATKEY_TEXTURE_OPACITY(0),&szPath))
		{
			LoadTexture(&pcMesh->piOpacityTexture,pAsset,&szPath);
		}
		else
		{
			int flags = 0;
			aiGetMaterialInteger(pcMat,AI_MATKEY_TEXFLAGS_DIFFUSE(0),&flags);

			// try to find out whether the diffuse texture has any
			// non-opaque pixels. If we find a few, use it as opacity texture
			if (pcMesh->piDiffuseTexture && !(flags & aiTextureFlags_IgnoreAlpha) && HasAlphaPixels(pcMesh->piDiffuseTexture))
			{
				int iVal;

				// NOTE: This special value is set by the tree view if the user
				// manually removes the alpha texture from the view ...
				if (AI_SUCCESS != aiGetMaterialInteger(pcMat,"no_a_from_d",0,0,&iVal))
				{
					pcMesh->piOpacityTexture = pcMesh->piDiffuseTexture;
					pcMesh->piOpacityTexture->AddRef();
				}
			}
		}
		// Ambient Texture
		if(AI_SUCCESS == aiGetMaterialString(pcMat,AI_MATKEY_TEXTURE_AMBIENT(0),&szPath))
		{
			LoadTexture(&pcMesh->piAmbientTexture,pAsset,&szPath);
		}
		// Emissive Texture
		if(AI_SUCCESS == aiGetMaterialString(pcMat,AI_MATKEY_TEXTURE_EMISSIVE(0),&szPath))
		{
			LoadTexture(&pcMesh->piEmissiveTexture,pAsset,&szPath);
		}
		// Shininess Texture
		if(AI_SUCCESS == aiGetMaterialString(pcMat,AI_MATKEY_TEXTURE_SHININESS(0),&szPath))
		{
			LoadTexture(&pcMesh->piShininessTexture,pAsset,&szPath);
		}
		// Lightmap Texture
		if(AI_SUCCESS == aiGetMaterialString(pcMat,AI_MATKEY_TEXTURE_LIGHTMAP(0),&szPath))
		{
			LoadTexture(&pcMesh->piLightmapTexture,pAsset,&szPath);
		}
		//Normal / HeightMap (We're going to assume that heightmaps won't be used)
		bool bHM = false;
		if(AI_SUCCESS == aiGetMaterialString(pcMat,AI_MATKEY_TEXTURE_NORMALS(0),&szPath))
		{
			LoadTexture(&pcMesh->piNormalTexture,pAsset,&szPath);
		}
		else
		{
			if(AI_SUCCESS == aiGetMaterialString(pcMat,AI_MATKEY_TEXTURE_HEIGHT(0),&szPath))
			{
				LoadTexture(&pcMesh->piNormalTexture,pAsset,&szPath);
			}
			else bib = true;
			bHM = true;
		}
	}

	int two_sided = 0;
	aiGetMaterialInteger(pcMat,AI_MATKEY_TWOSIDED,&two_sided);
	pcMesh->bTwosided = (two_sided != 0);

	// check whether we have already a material using the same
	// shader. This will decrease loading time rapidly ...
	for (unsigned int i = 0; i < pAsset->paScene->mNumMeshes;++i)
	{
		if (pAsset->paScene->mMeshes[i] == pcSource)
		{
			break;
		}
		AssetHelper::MeshHelper* pc = pAsset->ppaMeshes[i];

		if  ((pcMesh->piDiffuseTexture != NULL ? true : false) != 
			(pc->piDiffuseTexture != NULL ? true : false))
			continue;
		if  ((pcMesh->piSpecularTexture != NULL ? true : false) != 
			(pc->piSpecularTexture != NULL ? true : false))
			continue;
		if  ((pcMesh->piAmbientTexture != NULL ? true : false) != 
			(pc->piAmbientTexture != NULL ? true : false))
			continue;
		if  ((pcMesh->piEmissiveTexture != NULL ? true : false) != 
			(pc->piEmissiveTexture != NULL ? true : false))
			continue;
		if  ((pcMesh->piNormalTexture != NULL ? true : false) != 
			(pc->piNormalTexture != NULL ? true : false))
			continue;
		if  ((pcMesh->piOpacityTexture != NULL ? true : false) != 
			(pc->piOpacityTexture != NULL ? true : false))
			continue;
		if  ((pcMesh->piShininessTexture != NULL ? true : false) != 
			(pc->piShininessTexture != NULL ? true : false))
			continue;
		if  ((pcMesh->piLightmapTexture != NULL ? true : false) != 
			(pc->piLightmapTexture != NULL ? true : false))
			continue;
		if ((pcMesh->eShadingMode != aiShadingMode_Gouraud ? true : false) != 
			(pc->eShadingMode != aiShadingMode_Gouraud ? true : false))
			continue;

		if ((pcMesh->fOpacity != 1.0f ? true : false) != (pc->fOpacity != 1.0f ? true : false))
			continue;

		if (pcSource->HasBones() != pAsset->paScene->mMeshes[i]->HasBones())
			continue;

		// we can reuse this material
		if (pc->piEffect)
		{
			pcMesh->piEffect = pc->piEffect;
			pc->bSharedFX = pcMesh->bSharedFX = true;
			pcMesh->piEffect->AddRef();
			return 2;
		}
	}
	m_iShaderCount++;

	// Build the macros for the HLSL Compiler
	UINT iCurrent = 0;
	if(pcMesh->piDiffuseTexture)
	{
		sMacro[iCurrent].Name = "AV_DIFFUSE_TEXTURE";
		sMacro[iCurrent].Definition = "1";
		++iCurrent;

		if(mapU == aiTextureMapMode_Wrap)
			sMacro[iCurrent].Name = "AV_WRAPU";
		else if(mapU == aiTextureMapMode_Mirror)
			sMacro[iCurrent].Name = "AV_MIRRORU";
		else
			sMacro[iCurrent].Name = "AV_CLAMPU";

		sMacro[iCurrent].Definition = "1";
		++iCurrent;

		if(mapV == aiTextureMapMode_Wrap)
			sMacro[iCurrent].Name = "AV_WRAPV";
		else if(mapV == aiTextureMapMode_Mirror)
			sMacro[iCurrent].Name = "AV_MIRRORV";
		else
			sMacro[iCurrent].Name = "AV_CLAMPV";

		sMacro[iCurrent].Definition = "1";
		++iCurrent;
	}
	if(pcMesh->piSpecularTexture)
	{
		sMacro[iCurrent].Name = "AV_SPECULAR_TEXTURE";
		sMacro[iCurrent].Definition = "1";
		++iCurrent;
	}
	if(pcMesh->piAmbientTexture)
	{
		sMacro[iCurrent].Name = "AV_AMBIENT_TEXTURE";
		sMacro[iCurrent].Name = "1";
		++iCurrent;
	}
	if(pcMesh->piEmissiveTexture)
	{
		sMacro[iCurrent].Name = "AV_EMISSIVE_TEXTURE";
		sMacro[iCurrent].Definition = "1";
		++iCurrent;
	}
	char buf[32];
	if(pcMesh->piLightmapTexture)
	{
		sMacro[iCurrent].Name = "AV_LIGHTMAP_TEXTURE";
		sMacro[iCurrent].Definition = "1";
		++iCurrent;

		int idx;
		if(AI_SUCCESS == aiGetMaterialInteger(pcMat,AI_MATKEY_UVWSRC_LIGHTMAP(0),&idx) && idx >= 1 && pcSource->mTextureCoords[idx])
		{
			sMacro[iCurrent].Name = "AV_TWO_UV";
			sMacro[iCurrent].Definition = "1";
			++iCurrent;

			sMacro[iCurrent].Definition = "IN.TexCoord1";
		}
		else sMacro[iCurrent].Definition = "IN.TexCoord0";
		sMacro[iCurrent].Name = "AV_LIGHTMAP_TEXTURE_UV_COORD";

		++iCurrent;
		float f = 1.f;
		aiGetMaterialFloat(pcMat,AI_MATKEY_TEXBLEND_LIGHTMAP(0),&f);
		sprintf(buf, "%f", f);

		sMacro[iCurrent].Name = "LM_STRENGTH";
		sMacro[iCurrent].Definition = buf;
		++iCurrent;
	}
	if(pcMesh->piNormalTexture && !bib)
	{
		sMacro[iCurrent].Name = "AV_NORMAL_TEXTURE";
		sMacro[iCurrent].Definition = "1";
		++iCurrent;
	}
	if(pcMesh->piOpacityTexture)
	{
		sMacro[iCurrent].Name = "AV_OPACITY_TEXTURE";
		sMacro[iCurrent].Definition = "1";
		++iCurrent;

		if(pcMesh->piOpacityTexture == pcMesh->piDiffuseTexture)
		{
			sMacro[iCurrent].Name = "AV_OPACITY_TEXTURE_REGISTER_MASK";
			sMacro[iCurrent].Definition = "a";
			++iCurrent;
		}
		else
		{
			sMacro[iCurrent].Name = "AV_OPACITY_TEXTURE_REGISTER_MASK";
			sMacro[iCurrent].Definition = "r";
			++iCurrent;
		}
	}

	if(pcMesh->eShadingMode != aiShadingMode_Gouraud)
	{
		sMacro[iCurrent].Name = "AV_SPECULAR_COMPONENT";
		sMacro[iCurrent].Definition = "1";
		++iCurrent;

		if(pcMesh->piShininessTexture)
		{
			sMacro[iCurrent].Name = "AV_SHININESS_TEXTURE";
			sMacro[iCurrent].Definition = "1";
			++iCurrent;
		}
	}

	if(1.0f != pcMesh->fOpacity)
	{
		sMacro[iCurrent].Name = "AV_OPACITY";
		sMacro[iCurrent].Definition = "1";
		++iCurrent;
	}

	if(pcSource->HasBones())
	{
		sMacro[iCurrent].Name = "AV_SKINNING";
		sMacro[iCurrent].Definition = "1";
		++iCurrent;
	}

	sMacro[iCurrent].Name = NULL;
	sMacro[iCurrent].Definition = NULL;

	//load strings for shader
	char* temp;
	textFileRead("Material.hlsl", &temp);

	//compile!
	if(FAILED(D3DXCreateEffect(DirectXRenderer::getInstance()->getDevice(),
		temp, (UINT)strlen(temp)+1,(const D3DXMACRO*)sMacro, NULL, 0, NULL, &pcMesh->piEffect, &piBuffer)))
	{
		//failed to compile
		if(piBuffer)
		{
			LogManager::getInstance()->trace("[ERROR] HLSL: %s", (LPCSTR)piBuffer->GetBufferPointer());
			piBuffer->Release();
		}
		//use default instead
		textFileRead("Default.hlsl", &temp);
		if(FAILED(D3DXCreateEffect(DirectXRenderer::getInstance()->getDevice(),
			temp, (UINT)strlen(temp)+1, NULL, NULL, D3DXSHADER_USE_LEGACY_D3DX9_31_DLL,
			NULL, &pcMesh->piEffect, &piBuffer)))
		{
			if(piBuffer)
			{
				LogManager::getInstance()->trace("[ERROR] HLSL: %s", (LPCSTR)piBuffer->GetBufferPointer());
				piBuffer->Release();
				piBuffer = NULL;
				LogManager::getInstance()->trace("[ERROR] Something went seriously wrong");
				return 0;
			}
		}
		piBuffer->AddRef();

		if(AI_SUCCESS == aiGetMaterialString(pcMat, AI_MATKEY_NAME, &szPath) &&
			'\0' != szPath.data[0])
		{
			LogManager::getInstance()->trace("[ERROR] Unable to load material: %s", szPath.data);
		}
		else
		{
			LogManager::getInstance()->trace("Unable to load material: UNNAMED");
		}
		return 0;
	}
	else
	{
		if(DirectXRenderer::getInstance()->getCaps().PixelShaderVersion < D3DPS_VERSION(2,0))
			pcMesh->piEffect->SetTechnique("MaterialFX_FF");
	}

	if(piBuffer) piBuffer->Release();

	//commit all constants to the shader

	if(1.0f != pcMesh->fOpacity)
		pcMesh->piEffect->SetFloat("TRANSPARENCY",pcMesh->fOpacity);
	if(pcMesh->eShadingMode != aiShadingMode_Gouraud)
	{
		pcMesh->piEffect->SetFloat("SPECULARITY",pcMesh->fShininess);
		pcMesh->piEffect->SetFloat("SPECULAR_STRENGTH",pcMesh->fSpecularStrength);
	}
	pcMesh->piEffect->SetVector("DIFFUSE_COLOR",&pcMesh->vDiffuseColor);
	pcMesh->piEffect->SetVector("SPECULAR_COLOR",&pcMesh->vSpecularColor);
	pcMesh->piEffect->SetVector("AMBIENT_COLOR",&pcMesh->vAmbientColor);
	pcMesh->piEffect->SetVector("EMISSIVE_COLOR",&pcMesh->vEmissiveColor);

	if (pcMesh->piDiffuseTexture)
		pcMesh->piEffect->SetTexture("DIFFUSE_TEXTURE",pcMesh->piDiffuseTexture);
	if (pcMesh->piOpacityTexture)
		pcMesh->piEffect->SetTexture("OPACITY_TEXTURE",pcMesh->piOpacityTexture);
	if (pcMesh->piSpecularTexture)
		pcMesh->piEffect->SetTexture("SPECULAR_TEXTURE",pcMesh->piSpecularTexture);
	if (pcMesh->piAmbientTexture)
		pcMesh->piEffect->SetTexture("AMBIENT_TEXTURE",pcMesh->piAmbientTexture);
	if (pcMesh->piEmissiveTexture)
		pcMesh->piEffect->SetTexture("EMISSIVE_TEXTURE",pcMesh->piEmissiveTexture);
	if (pcMesh->piNormalTexture)
		pcMesh->piEffect->SetTexture("NORMAL_TEXTURE",pcMesh->piNormalTexture);
	if (pcMesh->piShininessTexture)
		pcMesh->piEffect->SetTexture("SHININESS_TEXTURE",pcMesh->piShininessTexture);
	if (pcMesh->piLightmapTexture)
		pcMesh->piEffect->SetTexture("LIGHTMAP_TEXTURE",pcMesh->piLightmapTexture);

	return 1;
}

int MaterialManager::SetupMaterial(AssetHelper::MeshHelper* pcMesh,
								   const aiMatrix4x4& pcProj,
								   const aiMatrix4x4& aiMe,
								   const aiMatrix4x4& pcCam)
{
	ai_assert(NULL != pcMesh);
	if(!pcMesh->piEffect) return 0;

	ID3DXEffect* piEnd = pcMesh->piEffect;

	piEnd->SetMatrix("WorldViewProjection", (const D3DXMATRIX*)&pcProj);
	piEnd->SetMatrix("World", (const D3DXMATRIX*)&aiMe);
	piEnd->SetMatrix("WorldInverseTranspose", (const D3DXMATRIX*)&pcCam);

	D3DXVECTOR4 apcVec[5];
	memset(apcVec,0,sizeof(apcVec));
	apcVec[0].x = Light::getInstance()->getDirection().GetX();
	apcVec[0].y = Light::getInstance()->getDirection().GetY();
	apcVec[0].z = Light::getInstance()->getDirection().GetZ();
	apcVec[0].w = 0.0f;
	apcVec[1].x = Light::getInstance()->getDirection().GetX() * -1.0f;
	apcVec[1].y = Light::getInstance()->getDirection().GetY() * -1.0f;
	apcVec[1].z = Light::getInstance()->getDirection().GetZ() * -1.0f;
	apcVec[1].w = 0.0f;
	D3DXVec4Normalize(&apcVec[0], &apcVec[0]);
	D3DXVec4Normalize(&apcVec[1], &apcVec[1]);
	piEnd->SetVectorArray("afLightDir", apcVec, 5);

	apcVec[0].x = Light::getInstance()->getDiffuse().GetRedFloat();
	apcVec[0].y = Light::getInstance()->getDiffuse().GetBlueFloat();
	apcVec[0].z = Light::getInstance()->getDiffuse().GetGreenFloat();
	apcVec[0].w = 1.0f;
	//No second light for now.
	apcVec[1].x = 0.0f;
	apcVec[1].y = 0.0f;
	apcVec[1].z = 0.0f;
	apcVec[1].w = 0.0f;
	piEnd->SetVectorArray("afLightColor",apcVec,5);

	apcVec[0].x = Light::getInstance()->getAmbient().GetRedFloat();
	apcVec[0].y = Light::getInstance()->getAmbient().GetGreenFloat();
	apcVec[0].z = Light::getInstance()->getAmbient().GetBlueFloat();
	apcVec[0].w = 1.0f;
	apcVec[1].x = Light::getInstance()->getAmbient().GetRedFloat();
	apcVec[1].y = Light::getInstance()->getAmbient().GetGreenFloat();
	apcVec[1].z = Light::getInstance()->getAmbient().GetBlueFloat();
	apcVec[1].w = 0.0f;
	piEnd->SetVectorArray("afLightColorAmbient",apcVec,5);

	apcVec[0].x = Camera::getInstance()->getPositionDX().x;
	apcVec[0].y = Camera::getInstance()->getPositionDX().y;
	apcVec[0].z = Camera::getInstance()->getPositionDX().z;
	piEnd->SetVector("vCameraPos",&apcVec[0]);

	if (pcMesh->bSharedFX)
	{
		// now commit all constants to the shader
		if (1.0f != pcMesh->fOpacity)
			pcMesh->piEffect->SetFloat("TRANSPARENCY",pcMesh->fOpacity);
		if (pcMesh->eShadingMode  != aiShadingMode_Gouraud)
		{
			pcMesh->piEffect->SetFloat("SPECULARITY",pcMesh->fShininess);
			pcMesh->piEffect->SetFloat("SPECULAR_STRENGTH",pcMesh->fSpecularStrength);
		}

		pcMesh->piEffect->SetVector("DIFFUSE_COLOR",&pcMesh->vDiffuseColor);
		pcMesh->piEffect->SetVector("SPECULAR_COLOR",&pcMesh->vSpecularColor);
		pcMesh->piEffect->SetVector("AMBIENT_COLOR",&pcMesh->vAmbientColor);
		pcMesh->piEffect->SetVector("EMISSIVE_COLOR",&pcMesh->vEmissiveColor);

		if (pcMesh->piOpacityTexture)
			pcMesh->piEffect->SetTexture("OPACITY_TEXTURE",pcMesh->piOpacityTexture);
		if (pcMesh->piDiffuseTexture)
			pcMesh->piEffect->SetTexture("DIFFUSE_TEXTURE",pcMesh->piDiffuseTexture);
		if (pcMesh->piSpecularTexture)
			pcMesh->piEffect->SetTexture("SPECULAR_TEXTURE",pcMesh->piSpecularTexture);
		if (pcMesh->piAmbientTexture)
			pcMesh->piEffect->SetTexture("AMBIENT_TEXTURE",pcMesh->piAmbientTexture);
		if (pcMesh->piEmissiveTexture)
			pcMesh->piEffect->SetTexture("EMISSIVE_TEXTURE",pcMesh->piEmissiveTexture);
		if (pcMesh->piNormalTexture)
			pcMesh->piEffect->SetTexture("NORMAL_TEXTURE",pcMesh->piNormalTexture);
		if (pcMesh->piShininessTexture)
			pcMesh->piEffect->SetTexture("SHININESS_TEXTURE",pcMesh->piShininessTexture);
		if (pcMesh->piLightmapTexture)
			pcMesh->piEffect->SetTexture("LIGHTMAP_TEXTURE",pcMesh->piLightmapTexture);
	}

	// setup the correct shader technique to be used for drawing
	if(DirectXRenderer::getInstance()->getCaps().PixelShaderVersion < D3DPS_VERSION(2,0))
	{
		piEnd->SetTechnique( "MaterialFXSpecular_FF");
	} 
	else if(DirectXRenderer::getInstance()->getCaps().PixelShaderVersion < D3DPS_VERSION(3,0))
	{
		piEnd->SetTechnique("MaterialFXSpecular_PS20_D1");
	}
	else
	{
		piEnd->SetTechnique("MaterialFXSpecular_D1");
	}

	UINT dwPasses = 0;
	piEnd->Begin(&dwPasses, 0);
	piEnd->BeginPass(0);
	return 1;
}

int MaterialManager::EndMaterial(AssetHelper::MeshHelper* pcMesh)
{
	ai_assert(NULL!=pcMesh);
	if(!pcMesh->piEffect)return 0;

	pcMesh->piEffect->EndPass();
	pcMesh->piEffect->End();

	return 1;
}