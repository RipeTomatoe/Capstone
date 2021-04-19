#ifndef __ASSETHELPER_H_
#define __ASSETHELPER_H_

#include "DirectXRenderer.h"
#include "Animator.h"
#include "assimp\include\assimp.h"
#include "assimp\include\aiScene.h"
#include "assimp\include\aiPostProcess.h"

class AssetHelper
{
public:
	enum 
	{
		// the original normal set will be used
		ORIGINAL = 0x0u,

		// a smoothed normal set will be used
		SMOOTH = 0x1u,

		// a hard normal set will be used
		HARD = 0x2u,
	};

	// default constructor
	AssetHelper()
		: iNormalSet(ORIGINAL)
	{
		//mAnimator = NULL;
		ppaMeshes = NULL;
		paScene = NULL;
	}

	~AssetHelper(void)
	{
		for(UINT i = 0; i < paScene->mNumMeshes; ++i)
		{
			delete ppaMeshes[i];
		}
	}

	//---------------------------------------------------------------
	// Helper class to store GPU related resources created for
	// a given aiMesh
	//---------------------------------------------------------------
	class MeshHelper
	{
	public:

		MeshHelper ()
			: 
			piVB				(NULL),
			piIB				(NULL),
			piEffect			(NULL),
			piDiffuseTexture	(NULL),
			piSpecularTexture	(NULL),
			piAmbientTexture	(NULL),
			piNormalTexture		(NULL),
			piEmissiveTexture	(NULL),
			piOpacityTexture	(NULL),
			piShininessTexture	(NULL),
			piLightmapTexture	(NULL),
			pvOriginalNormals	(NULL),
			bSharedFX(false),
			bTwosided (false){}

		~MeshHelper ()
		{
			if(piVB)
			{
				piVB->Release();
				piVB = NULL;
			}
			if(piIB)
			{
				piIB->Release();
				piIB = NULL;
			}
			if(piEffect)
			{
				piEffect->Release();
				piEffect = NULL;
			}
			if(piDiffuseTexture)
			{
				piDiffuseTexture->Release();
				piDiffuseTexture = NULL;
			}
			if(piNormalTexture)
			{
				piNormalTexture->Release();
				piNormalTexture = NULL;
			}
			if(piSpecularTexture)
			{
				piSpecularTexture->Release();
				piSpecularTexture = NULL;
			}
			if(piAmbientTexture)
			{
				piAmbientTexture->Release();
				piAmbientTexture = NULL;
			}
			if(piEmissiveTexture)
			{
				piEmissiveTexture->Release();
				piEmissiveTexture = NULL;
			}
			if(piOpacityTexture)
			{
				piOpacityTexture->Release();
				piOpacityTexture = NULL;
			}
			if(piShininessTexture)
			{
				piShininessTexture->Release();
				piShininessTexture = NULL;
			}
		}

		// shading mode to use. Either Lambert or otherwise phong
		// will be used in every case
		aiShadingMode eShadingMode;

		// vertex buffer
		IDirect3DVertexBuffer9* piVB;

		// index buffer. For partially transparent meshes
		// created with dynamic usage to be able to update
		// the buffer contents quickly
		IDirect3DIndexBuffer9* piIB;

		// shader to be used
		ID3DXEffect* piEffect;
		bool bSharedFX;

		// material textures
		IDirect3DTexture9* piDiffuseTexture;
		IDirect3DTexture9* piSpecularTexture;
		IDirect3DTexture9* piAmbientTexture;
		IDirect3DTexture9* piEmissiveTexture;
		IDirect3DTexture9* piNormalTexture;
		IDirect3DTexture9* piOpacityTexture;
		IDirect3DTexture9* piShininessTexture;
		IDirect3DTexture9* piLightmapTexture;

		// material colors
		D3DXVECTOR4 vDiffuseColor;
		D3DXVECTOR4 vSpecularColor;
		D3DXVECTOR4 vAmbientColor;
		D3DXVECTOR4 vEmissiveColor;

		// opacity for the material
		float fOpacity;

		// shininess for the material
		float fShininess;

		// strength of the specular highlight
		float fSpecularStrength;

		// two-sided?
		bool bTwosided;

		// Stores a pointer to the original normal set of the asset
		aiVector3D* pvOriginalNormals;
	};

	// One instance per aiMesh in the globally loaded asset
	MeshHelper** ppaMeshes;

	// Scene wrapper instance
	aiScene* paScene;

	// Animation player to animate the scene if necessary
	Animator* mAnimator;

	// Specifies the normal set to be used
	unsigned int iNormalSet;

	// ------------------------------------------------------------------
	// set the normal set to be used
	void SetNormalSet(unsigned int iSet);

	// ------------------------------------------------------------------
	// flip all normal vectors
	void FlipNormals(){}
	void FlipNormalsInt(){}
};

#endif //__ASSETHELPER_H_