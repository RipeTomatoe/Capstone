#include "Camera.h"
#include "RandomNumber.h"
#include "Colour.h"

Camera* Camera::pInstance = NULL;
Destroyer<Camera> Camera::destroyer;

//Constructor / Destructor
Camera::Camera(void)
{
	zxrot = yrot = 0;
	pos = new D3DXVECTOR3(0.0, 0.0, 0.0);
	at = new D3DXVECTOR3(1.0, 0.0, 0.0);
	up = new D3DXVECTOR3(0.0, 1.0, 0.0);
	player=NULL;
	portalFirst = NULL;
	portalSecond = NULL;
	bLastFrameHeld = false;
	bHold = false;

	node = NULL;
	dt = y = x = 0;

	DirectXRenderer::getInstance()->getHKWorld()->markForWrite();

	hkVector4 vertA(0,1.f,0);
	hkVector4 vertB(0,-3.f,0);

	hkpCapsuleShape* shape = new hkpCapsuleShape(vertA, vertB, 2.f);

	hkpCharacterRigidBodyCinfo info;
	info.m_mass = 200.f;
	info.m_shape = shape;
	info.m_maxLinearVelocity = 100.f;
	info.m_up.set(up->x, up->y, up->z);
	info.m_position.set(0.f, 15.f, 0.f);
	info.m_maxSlope = 70.f * HK_REAL_DEG_TO_RAD;
	
	player = new hkpCharacterRigidBody(info);
	player->getRigidBody()->setUserData(BLACK);

	shape->removeReference();

	hkpCharacterRigidBodyListener* listener = new hkpCharacterRigidBodyListener();
	player->setListener(listener);
	listener->removeReference();

	playerBody = player->getRigidBody();
	DirectXRenderer::getInstance()->getHKWorld()->addEntity(player->getRigidBody());

	//Create the state machine and context
	hkpCharacterState* state;
	hkpCharacterStateManager* manager = new hkpCharacterStateManager();

	state = new hkpCharacterStateOnGround();
	manager->registerState(state, HK_CHARACTER_ON_GROUND);
	state->removeReference();

	state = new hkpCharacterStateInAir();
	manager->registerState(state, HK_CHARACTER_IN_AIR);
	state->removeReference();

	state = new hkpCharacterStateJumping();
	manager->registerState(state, HK_CHARACTER_JUMPING);
	state->removeReference();

	state = new hkpCharacterStateClimbing();
	manager->registerState(state, HK_CHARACTER_CLIMBING);
	state->removeReference();

	playerContext = new hkpCharacterContext(manager, HK_CHARACTER_ON_GROUND);
	manager->removeReference();

	playerContext->setCharacterType(hkpCharacterContext::HK_CHARACTER_RIGIDBODY);
	playerContext->setFilterEnable(false);

	pGravGun = new hkpGravityGun();
	pGravGun->initGun(DirectXRenderer::getInstance()->getHKWorld());
	pGravGun->m_maxNumObjectsPicked = 1;
	pGravGun->m_maxDistOfObjectPicked = 20.f;
	pGravGun->m_throwVelocity = 20.f;
	pGravGun->m_maxMassOfObjectPicked = 200.f;
	pGravGun->m_capturedObjectPosition.set(10.f, 0.f, 0.f);

	DirectXRenderer::getInstance()->getHKWorld()->unmarkForWrite();

	SoundEngine::getInstance()->createSoundEffect("grass1.wav", &sGrass[0]);
	SoundEngine::getInstance()->createSoundEffect("grass2.wav", &sGrass[1]);
	SoundEngine::getInstance()->createSoundEffect("grass3.wav", &sGrass[2]);
	SoundEngine::getInstance()->createSoundEffect("grass4.wav", &sGrass[3]);
	SoundEngine::getInstance()->createSoundEffect("concrete1.wav", &sConcrete[0]);
	SoundEngine::getInstance()->createSoundEffect("concrete2.wav", &sConcrete[1]);
	SoundEngine::getInstance()->createSoundEffect("concrete3.wav", &sConcrete[2]);
	SoundEngine::getInstance()->createSoundEffect("concrete4.wav", &sConcrete[3]);
	SoundEngine::getInstance()->createSoundEffect("superphys_hold_loop.wav", &sPhysHold, true);
	SoundEngine::getInstance()->createSoundEffect("physcannon_drop.wav", &sPhysShoot);
	SoundEngine::getInstance()->createSoundEffect("physcannon_drop.wav", &sPhysDrop);
	SoundEngine::getInstance()->createSoundEffect("physcannon_pickup.wav", &sPhysPick);
	SoundEngine::getInstance()->createSoundEffect("physcannon_dryfire.wav", &sPhysDryFire);
	SoundEngine::getInstance()->createSoundEffect("portal_invalid_surface3.wav", &sPortInvalid);
	SoundEngine::getInstance()->createSoundEffect("portalgun_shoot_blue1.wav", &sPortShoot[0]);
	SoundEngine::getInstance()->createSoundEffect("portalgun_shoot_red1.wav", &sPortShoot[1]);
}
Camera::~Camera(void)
{
	delete pos; pos = NULL;
	delete at; at = NULL;
	delete up; up = NULL;
	DirectXRenderer::getInstance()->getHKWorld()->lock();
	DirectXRenderer::getInstance()->getHKWorld()->removeEntity(playerBody);
	DirectXRenderer::getInstance()->getHKWorld()->unlock();
}

//Update and Render
void Camera::update(double dTime)
{
	if(DirectXRenderer::getInstance()->getInput()->getMouseState().left)
	{
		DirectXRenderer::getInstance()->getHKWorld()->lock();

		{
			hkpWorldRayCastInput input;
			input.m_from.set(at->x, at->y, at->z);
			input.m_to.set(((at->x - pos->x)*100.f)+pos->x, ((at->y - pos->y)*100.f)+pos->y, ((at->z - pos->z)*100.f)+pos->z);

			hkpClosestRayHitCollector output;
			DirectXRenderer::getInstance()->getHKWorld()->castRay(input, output);

			if(output.hasHit())
			{
				hkpRigidBody* rb = hkpGetRigidBody(output.getHit().m_rootCollidable);
				hkUlong UD = rb->getUserData();
				if(RenderablePhysicsMap::getInstance()->theMap[UD] != NULL)
				{
					if(RenderablePhysicsMap::getInstance()->theMap[UD]->bPortal == true)
					{
						if(portalFirst != NULL)
						{
							portalFirst->setToDie();
						}
						hkVector4 hitPosition;
						hitPosition.setInterpolate4(input.m_from, input.m_to, output.getHit().m_hitFraction);
						Vector temp;
						temp.SetValues(output.getHit().m_normal.m_quad.v[0], output.getHit().m_normal.m_quad.v[1], output.getHit().m_normal.m_quad.v[2]);
						portalFirst = new Portal(Vector(hitPosition.m_quad.v[0], hitPosition.m_quad.v[1], hitPosition.m_quad.v[2]), temp);
						Portal::aPortals[0] = portalFirst;
						portalFirst->SetUserData(0);
						DirectXRenderer::getInstance()->getActiveScene()->objToRender.push_front(portalFirst);
						SoundEngine::getInstance()->playSoundEffect(sPortShoot[0]);
					}
					else
					{
						SoundEngine::getInstance()->playSoundEffect(sPortInvalid);
					}
				}
				//LogManager::getInstance()->trace("Ray hit at (%f, %f, %f)", hitPosition.m_quad.v[0], hitPosition.m_quad.v[1], hitPosition.m_quad.v[2]);
			}
			else
			{
				SoundEngine::getInstance()->playSoundEffect(sPortInvalid);
				//LogManager::getInstance()->trace("No collision detected");
			}
		}

		DirectXRenderer::getInstance()->getHKWorld()->unlock();
	}

	if(DirectXRenderer::getInstance()->getInput()->getMouseState().right)
	{
		DirectXRenderer::getInstance()->getHKWorld()->lock();

		{
			hkpWorldRayCastInput input;
			input.m_from.set(at->x, at->y, at->z);
			input.m_to.set(((at->x - pos->x)*100.f)+pos->x, ((at->y - pos->y)*100.f)+pos->y, ((at->z - pos->z)*100.f)+pos->z);

			hkpClosestRayHitCollector output;
			DirectXRenderer::getInstance()->getHKWorld()->castRay(input, output);

			if(output.hasHit())
			{
				hkpRigidBody* rb = hkpGetRigidBody(output.getHit().m_rootCollidable);
				hkUlong UD = rb->getUserData();
				if(RenderablePhysicsMap::getInstance()->theMap[UD] != NULL)
				{
					if(RenderablePhysicsMap::getInstance()->theMap[UD]->bPortal == true)
					{
						if(portalSecond != NULL)
						{
							portalSecond->setToDie();
						}
						hkVector4 hitPosition;
						hitPosition.setInterpolate4(input.m_from, input.m_to, output.getHit().m_hitFraction);
						Vector temp;
						temp.SetValues(output.getHit().m_normal.m_quad.v[0], output.getHit().m_normal.m_quad.v[1], output.getHit().m_normal.m_quad.v[2]);
						portalSecond = new Portal(Vector(hitPosition.m_quad.v[0], hitPosition.m_quad.v[1], hitPosition.m_quad.v[2]), temp);
						Portal::aPortals[1] = portalSecond;
						portalSecond->SetUserData(1);
						DirectXRenderer::getInstance()->getActiveScene()->objToRender.push_front(portalSecond);
						SoundEngine::getInstance()->playSoundEffect(sPortShoot[0]);
					}
					else
					{
						SoundEngine::getInstance()->playSoundEffect(sPortInvalid);
					}
				}
				//LogManager::getInstance()->trace("Ray hit at (%f, %f, %f)", hitPosition.m_quad.v[0], hitPosition.m_quad.v[1], hitPosition.m_quad.v[2]);
			}
			else
			{
				SoundEngine::getInstance()->playSoundEffect(sPortInvalid);
				//LogManager::getInstance()->trace("No collision detected");
			}
		}

		DirectXRenderer::getInstance()->getHKWorld()->unlock();
	}

	DirectXRenderer::getInstance()->getHKWorld()->lock();
	
	hkpCharacterInput input;
	hkpCharacterOutput output;
	{
		if(DirectXRenderer::getInstance()->getInput()->isKeyHeld(KEY_W))
			input.m_inputUD = -1;
		else if(DirectXRenderer::getInstance()->getInput()->isKeyHeld(KEY_S))
			input.m_inputUD = 1;
		else input.m_inputUD = 0;

		if(DirectXRenderer::getInstance()->getInput()->isKeyHeld(KEY_A))
			input.m_inputLR = -1;
		else if(DirectXRenderer::getInstance()->getInput()->isKeyHeld(KEY_D))
			input.m_inputLR = 1;
		else input.m_inputLR = 0;

		static int walk = 0;

		if(input.m_inputLR || input.m_inputUD)
		{
			if(playerContext->getState() == HK_CHARACTER_ON_GROUND)
			{
				bool isPlay;
				SoundEngine::getInstance()->getSoundEffectChannel()->isPlaying(&isPlay);
				if(!isPlay)
				{
					if(walk > 3 || walk < 0) walk = 0;
					SoundEngine::getInstance()->playSoundEffect(sConcrete[walk]);
					walk++;
				}
			}
		}

		static bool jump = false;

		if(playerContext->getState() == HK_CHARACTER_IN_AIR)
		{
			jump = true;
		}
		if(jump == true)
		{
			if(playerContext->getState() != HK_CHARACTER_IN_AIR)
			{
				bool isPlay;
				SoundEngine::getInstance()->getSoundEffectChannel()->isPlaying(&isPlay);
				if(!isPlay)
				{
					SoundEngine::getInstance()->playSoundEffect(sConcrete[0]);
					jump = false;
				}
			}
		}

		input.m_wantJump = DirectXRenderer::getInstance()->getInput()->isKeyDown(KEY_SPACE);
		input.m_atLadder = false;

		input.m_up.set(up->x, up->y, up->z);
		input.m_forward.set((at->x - pos->x), (at->y - pos->y), (at->z - pos->z));

		hkStepInfo stepInfo;
		stepInfo.m_deltaTime = (hkTime)(dTime/1000.f);
		stepInfo.m_invDeltaTime = (hkTime)(1.f/(dTime/1000.f));

		input.m_stepInfo = stepInfo;

		input.m_characterGravity.set(0,(-9.8f)*2.f,0);
		input.m_velocity = player->getRigidBody()->getLinearVelocity();
		input.m_position = player->getRigidBody()->getPosition();
		pos = new D3DXVECTOR3(input.m_position.m_quad.v);

		player->checkSupport(stepInfo, input.m_surfaceInfo);

		playerContext->update(input, output);
		player->setLinearVelocity(output.m_velocity, (hkReal)(dTime/1000.f));
	}
	
	DirectXRenderer::getInstance()->getHKWorld()->unlock();

	DirectXRenderer::getInstance()->getHKWorld()->lock();

	{
		hkTransform transform;
		transform.setTranslation(hkVector4(pos->x, pos->y, pos->z));

		hkVector4 x(at->x - pos->x, at->y - pos->y, at->z - pos->z);
		hkVector4 y(up->x, up->y, up->z);
		hkVector4 z;

		z.setCross(x,y); z.normalize3();
		y.setCross(z,x); y.normalize3();
		transform.getRotation().setCols(x,y,z);

		if(DirectXRenderer::getInstance()->getInput()->isKeyDown(KEY_E))
		{
			bHold = true;
		}

		pGravGun->stepGun(	(hkReal)(dTime/1000.0), 
							DirectXRenderer::getInstance()->getHKWorld(), 
							player->getRigidBody(), 
							transform, 
							false,//DirectXRenderer::getInstance()->getInput()->isKeyDown(KEY_Q), 
							bHold);

		bHold = false;
	}

	DirectXRenderer::getInstance()->getHKWorld()->unlock();

	if(pGravGun->m_grabbedBodies.getSize() > 0)
	{
		if(bLastFrameHeld == false)
		{
			SoundEngine::getInstance()->getSoundSystem()->playSound(FMOD_CHANNEL_FREE, sPhysPick, false, &cGunChannel);
			cGunChannel->setVolume(0.3f);
		}
		else
		{
			bool temp;
			cGunChannel->isPlaying(&temp);
			if(!temp) SoundEngine::getInstance()->getSoundSystem()->playSound(FMOD_CHANNEL_FREE, sPhysHold, false, &cGunChannel);
			cGunChannel->setVolume(0.05f);
		}
		bLastFrameHeld = true;
	}
	else if(bLastFrameHeld == true)
	{
		cGunChannel->stop();
		if(DirectXRenderer::getInstance()->getInput()->isKeyHeld(KEY_E))
		{
			SoundEngine::getInstance()->getSoundSystem()->playSound(FMOD_CHANNEL_FREE, sPhysDrop, false, &cGunChannel);
			cGunChannel->setVolume(0.3f);
		}
		bLastFrameHeld = false;
	}
	else if(DirectXRenderer::getInstance()->getInput()->isKeyHeld(KEY_E))
	{
		bool temp;
		cGunChannel->isPlaying(&temp);
		if(!temp) SoundEngine::getInstance()->getSoundSystem()->playSound(FMOD_CHANNEL_FREE, sPhysDryFire, false, &cGunChannel);
		cGunChannel->setVolume(0.3f);
	}

	FMOD_VECTOR temppos, tempvel, tempat, tempup;
	temppos.x = pos->x;
	temppos.y = pos->y;
	temppos.z = pos->z;
	tempvel.x = tempvel.y = tempvel.z = 0;
	tempat.x = at->x - pos->x;
	tempat.y = at->y - pos->y;
	tempat.z = at->z - pos->z;
	tempup.x = up->x;
	tempup.y = up->y;
	tempup.z = up->z;
	SoundEngine::getInstance()->getSoundSystem()->set3DListenerAttributes(0, &temppos, &tempvel, &tempat, &tempup);

	dt += (float)(dTime/1000.0);
	if(dt >= (2.f*D3DX_PI))
		dt -= (2.f*D3DX_PI);
	yrot -= (float)(DirectXRenderer::getInstance()->getInput()->getMouseState().x * 0.008);
	zxrot -= (float)(DirectXRenderer::getInstance()->getInput()->getMouseState().y * 0.005);

	y = (sin(dt)*(0.00016f));
	x = (cos(dt)*(0.00004f));

	//This prevents the player from turning past the float limit, where numbers become less accurate. Although unlikely, I still wanted to prevent this.
	if(yrot >= (2*D3DX_PI))
		yrot -= (2*D3DX_PI);
	if(yrot <= (-2*D3DX_PI))
		yrot += (2*D3DX_PI);

	//This test is for the upper and lower limits of the camera.
	if(zxrot <= -D3DX_PI/2.f + 0.01f)
		zxrot = -D3DX_PI/2.f + 0.01f;
	if(zxrot >= D3DX_PI/2.f - 0.01f)
		zxrot = D3DX_PI/2.f - 0.01f;

	zxrot -= y;
	yrot -= x;

	at->x = (sin(yrot) * cos(zxrot) + pos->x);
	at->y = sin(zxrot) + pos->y;
	at->z = (-cos(yrot) * cos(zxrot) + pos->z);

	/*hkpCharacterStateType state = playerContext->getState();
	const char * stateStr;

	switch (state)
	{
	case HK_CHARACTER_ON_GROUND:
		stateStr = "On Ground";	break;
	case HK_CHARACTER_JUMPING:
		stateStr = "Jumping"; break;
	case HK_CHARACTER_IN_AIR:
		stateStr = "In Air"; break;
	case HK_CHARACTER_CLIMBING:
		stateStr = "Climbing"; break;
	default:
		stateStr = "Other";	break;
	}
	char temp[255];
	sprintf_s(temp, "Character State: %s", stateStr);
	LogManager::getInstance()->trace(temp);*/
}
void Camera::render(void)
{
	DirectXRenderer::getInstance()->getLightingEffect()->SetVector("CameraPosition", &D3DXVECTOR4(pos->x, pos->y, pos->z, 0));
	
	D3DXMATRIX matView;
	D3DXMatrixLookAtLH(&matView, pos, at, up);
	DirectXRenderer::getInstance()->getDevice()->SetTransform(D3DTS_VIEW, &matView);

	float aspectRatio = ((float)DirectXRenderer::getInstance()->getWidth() / (float)DirectXRenderer::getInstance()->getHeight());
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(70), aspectRatio, 0.1f, 1000.0f);
	DirectXRenderer::getInstance()->getDevice()->SetTransform(D3DTS_PROJECTION, &matProj);

	//update L and R node's positions and orientations
	if(node != NULL)
	{
		Vector position(pos->x, pos->y, pos->z);
		node->setPosition(position);
		Vector offset(-1.f, -1.f, -1.25f);
		node->setOffset(offset);
		Vector rotation;
		rotation.SetValues(sin(D3DX_PI/2.f) * cos(zxrot), 0, -cos(D3DX_PI/2.f) * cos(zxrot), zxrot);
		node->setRotation(rotation);
		node->vecRotation2.SetValues(0,1,0,-yrot);
	}
}

//Nodes
void Camera::setNode(SceneNode* right){node=right;}

void Camera::MoveX(float x){pos->x+=x;}
void Camera::MoveY(float y){pos->y+=y;}
void Camera::MoveZ(float z){pos->z+=z;}
void Camera::setPosition(float x, float y, float z){*pos=D3DXVECTOR3(x,y,z);}
void Camera::setLookAt(float x, float y, float z){*at=D3DXVECTOR3(x,y,z);}
void Camera::setUp(float x, float y, float z){*up=D3DXVECTOR3(x,y,z);}

//Returns
D3DXVECTOR3 Camera::getPositionDX(void)const{return *pos;}
D3DXVECTOR3 Camera::getLookAtDX(void)const{return *at;}