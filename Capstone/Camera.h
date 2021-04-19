#ifndef __CAMERA_H_
#define __CAMERA_H_

#include "DirectXRenderer.h"
#include "Destroyer.h"
#include "SceneNode.h"
#include "Portal.h"
//#include "SoundEngine.h"

class Camera
{
	friend class Destroyer<Camera>;
public:
	/********************************************************************************************
	Summary:	Returns the instance of the Camera singleton. 
				If it doesn't exist it creates a new instance and sets the destroyer object.
	Parameters: None.
	********************************************************************************************/
	static inline Camera* getInstance(void)
	{
		if(pInstance == NULL)
		{
			pInstance = new Camera();
			//destroyer.setSingleton(pInstance);
		}
		return pInstance;
	}

	/********************************************************************************************
	Summary:	Updates the camera "Look At" and adds the "Breath" function
	Parameters: [in] dTime - The time between the last frame and the current frame.
	********************************************************************************************/
	void update(double dTime);

	/********************************************************************************************
	Summary:	Applies all the Camera transformations and renders any attached SceneNode's
	Parameters: None.
	********************************************************************************************/
	void render(void);

	/********************************************************************************************
	Summary:	Sets the position vector.
	Parameters: [in] x - The x co-ordinate of the position vector.
				[in] y - The y co-ordinate of the position vector.
				[in] z - The z co-ordinate of the position vector.
	********************************************************************************************/
	void setPosition(float x, float y, float z);

	/********************************************************************************************
	Summary:	Sets the Look At vector.
	Parameters: [in] x - The x co-ordinate of the Look At vector.
				[in] y - The y co-ordinate of the Look At vector.
				[in] z - The z co-ordinate of the Look At vector.
	********************************************************************************************/
	void setLookAt(float x, float y, float z);

	/********************************************************************************************
	Summary:	Sets the up vector (value between 1 and 0).
	Parameters: [in] x - The x co-ordinate of the up vector.
				[in] y - The y co-ordinate of the up vector.
				[in] z - The z co-ordinate of the up vector.
	********************************************************************************************/
	void setUp(float x, float y, float z);

	/********************************************************************************************
	Summary:	Moves the Camera in the X direction.
	Parameters: [in] x - Value to move by.
	********************************************************************************************/
	void MoveX(float x);

	/********************************************************************************************
	Summary:	Moves the Camera in the Y direction.
	Parameters: [in] y - Value to move by.
	********************************************************************************************/
	void MoveY(float y);

	/********************************************************************************************
	Summary:	Moves the Camera in the Z direction.
	Parameters: [in] z - Value to move by.
	********************************************************************************************/
	void MoveZ(float z);

	/********************************************************************************************
	Summary:	Returns the DirectX Vector for position.
	Parameters: None.
	Returns:	D3DXVECTOR3
	********************************************************************************************/
	D3DXVECTOR3 getPositionDX(void) const;

	/********************************************************************************************
	Summary:	Returns the DirectX Vector for "Look At".
	Parameters: None.
	Returns:	D3DXVECTOR3
	********************************************************************************************/
	D3DXVECTOR3 getLookAtDX(void) const;

	/********************************************************************************************
	Summary:	Sets the right Camera SceneNode
	Parameters: [in] right - a pointer to a SceneNode
	********************************************************************************************/
	void setNode(SceneNode* right);

	hkpRigidBody* getRigidBody(void){return player->getRigidBody();}

	void dropObject(void)
	{
		bHold = true;
	}

	bool isHeld(void)
	{
		return bLastFrameHeld;
	}

	static void reset(void)
	{
		if(pInstance != NULL)
		{
			delete pInstance;
			pInstance = new Camera();
			//destroyer.setSingleton(pInstance);
		}
	}

protected:
	static Camera* pInstance;
	static Destroyer<Camera> destroyer;
	D3DXVECTOR3 *pos, *at, *up;
	float yrot, zxrot;
	SceneNode *node;
	float dt,y,x;
	hkpCharacterRigidBody* player;
	hkpRigidBody* playerBody;
	hkpCharacterContext* playerContext;
	hkpGravityGun* pGravGun;
	FMOD::Sound* sGrass[4], *sConcrete[4];
	FMOD::Sound *sPhysHold, *sPhysDrop, *sPhysShoot, *sPhysPick, *sPhysDryFire;
	FMOD::Sound *sPortOpen[3], *sPortShoot[2], *sPortInvalid;
	FMOD::Channel* cGunChannel;
	Portal *portalFirst, *portalSecond;
	bool bHold;
	bool bLastFrameHeld;

	/********************************************************************************************
	Summary:	Default Constructor
	Parameters: None.
	********************************************************************************************/
	Camera(void);

	/********************************************************************************************
	Summary:	Default Destructor.
	Parameters: None.
	********************************************************************************************/
	virtual ~Camera(void);
};

#endif //__CAMERA_H_