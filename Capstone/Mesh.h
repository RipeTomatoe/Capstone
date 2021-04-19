#ifndef __MESH_H_
#define __MESH_H_

#include "DirectXRenderer.h"
#include "assimp\include\aiScene.h"

class Mesh
{
public:
	LPDIRECT3DVERTEXBUFFER9	pVB;
	LPDIRECT3DINDEXBUFFER9	pIB;
	LPDIRECT3DTEXTURE9		pTexture;
	LPDIRECT3DTEXTURE9		pBump;
	UINT					mNumVerts; 
	UINT					mNumFaces;

	/********************************************************************************************
	Summary:	Default Constructor.
	Parameters: None.
	********************************************************************************************/
	Mesh(void);

	/********************************************************************************************
	Summary:	Renders the current mesh.
	Parameters: [in] effect - the current effect.
	********************************************************************************************/
	void Render(ID3DXEffect* effect);

	/********************************************************************************************
	Summary:	Loads the diffuse and bump textures for the current mesh.
	Parameters: [in] scene - the Scene that the mesh resides in.
				[in] mesh - the ID for this mesh.
	********************************************************************************************/
	void loadTexture(aiScene* scene, UINT mesh);

	/********************************************************************************************
	Summary:	Default Destructor.
	Parameters: None.
	********************************************************************************************/
	virtual ~Mesh(void);
};

#endif //__MESH_H_