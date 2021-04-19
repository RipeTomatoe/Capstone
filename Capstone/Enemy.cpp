#include "Enemy.h"
#include "Camera.h"

Enemy::Enemy(const char* fileName, float x, float y, float z, SHAPE_TYPE type)
{
	char temp[255];
	sprintf_s(temp, "Resources/Models/%s", fileName);
	loadAsset(temp);

	DirectXRenderer::getInstance()->getHKWorld()->markForWrite();

	hkpCharacterRigidBodyCinfo info;

	if(type == CUBE)
	{
		hkpBoxShape* shape = new hkpBoxShape(hkVector4((hkReal)((vecMax.GetX()-vecMin.GetX())/2.f), (hkReal)((vecMax.GetY()-vecMin.GetY())/2.f), (hkReal)((vecMax.GetZ()-vecMin.GetZ())/2.f)));
		info.m_shape = shape;
	}
	else
	{
		hkVector4 vertA(0,vecMax.GetY(),0);
		hkVector4 vertB(0,vecMin.GetY(),0);

		float radius = vecMax.GetX() - vecMin.GetX();
		if((vecMax.GetZ()-vecMin.GetZ()) > radius)
			radius = (vecMax.GetZ()-vecMin.GetZ());
		hkpCapsuleShape* shape = new hkpCapsuleShape(vertA, vertB, radius);
		info.m_shape = shape;
	}

	info.m_mass = 100.f;
	info.m_maxForce = 800.f;
	info.m_up.set(0,1,0);
	info.m_position.set(x,y,z);

	pBody = new hkpCharacterRigidBody(info);
	
	//shape->removeReference();

	hkpCharacterRigidBodyListener* listener = new hkpCharacterRigidBodyListener();
	pBody->setListener(listener);
	listener->removeReference();

	DirectXRenderer::getInstance()->getHKWorld()->addEntity(pBody->getRigidBody());

	//create the state machine and context
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

	pBodyContext = new hkpCharacterContext(manager, HK_CHARACTER_ON_GROUND);
	manager->removeReference();

	pBodyContext->setCharacterType(hkpCharacterContext::HK_CHARACTER_RIGIDBODY);

	DirectXRenderer::getInstance()->getHKWorld()->unmarkForWrite();

	pBehaviour = new AIWander();
}

Enemy::~Enemy(void)
{
	
}

void Enemy::Update(double dTime)
{
	//Do AIBehaviour
	pBehaviour->Update(dTime);

	//update character
	DirectXRenderer::getInstance()->getHKWorld()->lock();
	
	hkpCharacterInput input;
	hkpCharacterOutput output;
	{
		input.m_inputUD = 1.f;
		input.m_inputLR = 0.f;

		input.m_wantJump = false;
		input.m_atLadder = false;

		input.m_up.set(0,1,0);
		input.m_forward = pBehaviour->getHKVector4Forward();

		//vecRotation.SetValues(0,1,0,pBehaviour->getAngle()-(HK_REAL_PI/2.0));
			
		hkStepInfo stepInfo;
		stepInfo.m_deltaTime = (hkTime)dTime;
		stepInfo.m_invDeltaTime = (hkTime)(1.f/(dTime));

		input.m_stepInfo = stepInfo;

		input.m_characterGravity.set(0,(-9.8f)*2.f,0);
		input.m_velocity = pBody->getRigidBody()->getLinearVelocity();
		input.m_position = pBody->getRigidBody()->getPosition();
		vecPosition.SetValues(input.m_position.m_quad.v[0], input.m_position.m_quad.v[1], input.m_position.m_quad.v[2]);

		pBody->checkSupport(stepInfo, input.m_surfaceInfo);

		pBodyContext->update(input, output);
		pBody->setLinearVelocity(output.m_velocity, (hkReal)dTime);
	}
	
	DirectXRenderer::getInstance()->getHKWorld()->unlock();
}

void Enemy::Render(void)
{
	DirectXRenderer::getInstance()->pushMatrix();
	DirectXRenderer::getInstance()->scale(fScale);
	DirectXRenderer::getInstance()->rotateAxis(vecRotation, vecRotation.GetW());
	DirectXRenderer::getInstance()->translate(vecPosition);
	DirectXRenderer::getInstance()->updateLighting();
	ID3DXEffect* effect = DirectXRenderer::getInstance()->getLightingEffect();
	for(UINT i = 0; i < mNumMeshes; ++i)
	{
		vpMeshes[i]->Render(effect);
	}
	DirectXRenderer::getInstance()->popMatrix();
}