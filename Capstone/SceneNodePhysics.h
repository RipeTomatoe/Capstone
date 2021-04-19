#ifndef __SCENENODEPHYSICS_H_
#define __SCENENODEPHYSICS_H_

//#include "SceneNode.h"
#include "RenderablePhysicsMap.h"
#include "Mesh.h"

class SceneNodePhysics : /*public SceneNode, */public RenderablePhysics
{
public:
	enum SCENENODE_TYPE
	{
		NODE_DYNAMIC = 0x01,
		NODE_STATIC = 0x02
	};
private:
	std::vector<Mesh*>	vpMeshes;
	UINT				mNumMeshes;
	Vector				vecPosition;
	Vector				vecRotation;
	Vector				vecOffset;
	Vector				vecMin, vecMax;
	float				fScale;
	void createRigidBody(SCENENODE_TYPE type, float mass);
	int loadAsset(const char* fileName);
public:
	SceneNodePhysics(const char* fileName, float x = 0.f, float y = 0.f, float z = 0.f, SCENENODE_TYPE type = NODE_STATIC, float mass = 100.f);
	~SceneNodePhysics(void);
	void Update(double dTime);
	void Render(void);
};

#endif //__SCENENODEPHYSICS_H_