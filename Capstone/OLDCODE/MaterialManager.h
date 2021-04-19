#ifndef __MATERIALMANAGER_H_
#define __MATERIALMANAGER_H_

#include "DirectXRenderer.h"
#include "Destroyer.h"
#include "AssetHelper.h"

class MaterialManager
{
private:
	friend class DirectXRenderer;
	friend class Destroyer<MaterialManager>;
	static MaterialManager* pInstance;
	static Destroyer<MaterialManager> destroyer;
	UINT m_iShaderCount;
	MaterialManager(void) : m_iShaderCount(0){};
	int SetDefaultTexture(IDirect3DTexture9** p_ppiOut);
	void HMtoNMIfNecessary(IDirect3DTexture9* piTexture, IDirect3DTexture9** piTextureOut, bool bWasOriginallyHM = true);
	bool HasAlphaPixels(IDirect3DTexture9* piTexture);
public:
	static inline MaterialManager* getInstance(void)
	{
		if(pInstance == NULL)
		{
			pInstance = new MaterialManager();
			destroyer.setSingleton(pInstance);
		}
		return pInstance;
	}
	int CreateMaterial(AssetHelper* pAsset, int matIndex);
	void DeleteMaterial(AssetHelper::MeshHelper* pcIn);
	int SetupMaterial(AssetHelper::MeshHelper* pcMesh, const aiMatrix4x4& pcProj, const aiMatrix4x4& aiMe, const aiMatrix4x4&pcCam);
	int EndMaterial(AssetHelper::MeshHelper* pcMesh);
	int UpdateSpecularMaterials(AssetHelper* pAsset);
	int LoadTexture(IDirect3DTexture9** p_ppiOut, AssetHelper* pAsset, aiString* szPath);
	inline UINT GetShaderCount(void)
	{
		return this->m_iShaderCount;
	}
	inline void Reset(void)
	{
		this->m_iShaderCount = 0;
	}
	virtual ~MaterialManager(void){ }
};

#endif //__MATERIALMANAGER_H_