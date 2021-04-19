#ifndef __SCENENODE_H_
#define __SCENENODE_H_

#include "DirectXRenderer.h"
#include "Vector.h"
#include "Mesh.h"
#include "Renderable.h"

class SceneNode : public Renderable
{
	friend class SceneNodePhysics;
	friend class Camera;
protected:
	std::vector<Mesh*>	vpMeshes;
	UINT				mNumMeshes;
	Vector				vecPosition;
	Vector				vecRotation;
	Vector				vecRotation2; //ONLY FOR THE CAMERA
	Vector				vecOffset;
	Vector				vecMin, vecMax;
	float				fScale;

	/********************************************************************************************
	Summary:	Load's a model into meshes
	Parameters: [in] fileName - name of the file.
				[in] type - The physics type for the model (NODE_DYNAMIC or NODE_STATIC)
	********************************************************************************************/
	int loadAsset(const char* fileName);

public:

	/********************************************************************************************
	Summary:	Updates the node from the Havok world
	Parameters: [in] dTime - the time since last frame.
	********************************************************************************************/
	void Update(double dTime);

	/********************************************************************************************
	Summary:	Renders the current node
	Parameters: None.
	********************************************************************************************/
	void Render(void);

	void SetModel(const char* fileName);

	/********************************************************************************************
	Summary:	Set's the node to a new position (doesn't work with Havok)
	Parameters: [in] v - new Position Vector
	********************************************************************************************/
	void setPosition(Vector v) { vecPosition = v; }

	void setOffset(Vector v) { vecOffset = v; }

	/********************************************************************************************
	Summary:	Set's the uniform scale for the node 
					(only changes the graphical representation, not the physical)
	Parameters: [in] s - Uniform scaling
	********************************************************************************************/
	void setScale(float s) { fScale = s; }

	/********************************************************************************************
	Summary:	Set's rotation for the node (doesn't work with Havok)
	Parameters: [in] v - Set's the rotation
	********************************************************************************************/
	void setRotation(Vector v) { vecRotation = v; }

	/********************************************************************************************
	Summary:	Returns the uniform scale.
	Parameters: None.
	Returns:	float
	********************************************************************************************/
	float getScale(void) { return fScale; }

	/********************************************************************************************
	Summary:	Default Constructor
	Parameters: [in] fileName - name of the model file
				[in] type - physics type (dynamic or static)
				[in] x,y,z - starting position for the node
	********************************************************************************************/
	SceneNode(const char* fileName, float x = 0.f, float y = 0.f, float z = 0.f);

	/********************************************************************************************
	Summary:	Default Constructor (NULLs all values)
	Parameters: None.
	********************************************************************************************/
	SceneNode(void);

	/********************************************************************************************
	Summary:	Default Destructor.
	Parameters: None.
	********************************************************************************************/
	virtual ~SceneNode(void);
};

#endif //__SCENENODE_H_