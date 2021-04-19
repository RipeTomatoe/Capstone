#include "Portal.h"
#include "RandomNumber.h"
#include "Colour.h"
#include "Camera.h"

LPDIRECT3DTEXTURE9 Portal::pTexture[2];
LPDIRECT3DVERTEXBUFFER9 Portal::pVB = NULL;
Portal* Portal::aPortals[2];
FMOD::Sound* Portal::sPortEnter[2];

void PortalPhantom::phantomEnterEvent(const hkpCollidable* collidableA, const hkpCollidable* collidableB, const hkpCollisionInput& env)
{
	//remember the phantom is A
	//move B to the other portal and keep all momentum relative to it.
	hkpRigidBody* RBa = hkpGetRigidBody(collidableA);
	hkpRigidBody* RBb = hkpGetRigidBody(collidableB);

	hkUlong UDa = RBa->getUserData();
	if(UDa == 0 || UDa == 1)
	{
		if(Portal::aPortals[0] != NULL && Portal::aPortals[1] != NULL)
		{
			if(UDa == 0)
			{
				if(!left) return;
				Portal::aPortals[1]->pEvents->left = false;
				Vector temp = Portal::aPortals[1]->vPosition;
				Vector normal = Portal::aPortals[1]->vNormal;
				Vector normal2 = Portal::aPortals[0]->vNormal;
				normal.normalize();normal2.normalize();
				hkVector4 tempPos;
				if(RBb->getUserData() == BLACK)
				{
					tempPos.set(temp.GetX() + (normal.GetX()*7.f), temp.GetY() + (normal.GetY()*7.f), temp.GetZ() + (normal.GetZ()*7.f));
					Portal::aPortals[1]->pEvents->left = true;
				}
				else
				{
					tempPos.set(temp.GetX() + (normal.GetX()*1.1f), temp.GetY() + (normal.GetY()*1.1f), temp.GetZ() + (normal.GetZ()*1.1f));
				}
				hkVector4 velocity = RBb->getLinearVelocity();
				hkVector4 outVec = velocity;
				hkReal rotAngle = 0;
				if(normal.dot(normal2) >= 0.9)
				{
					if(normal.GetY() >= 0.9 || normal.GetY() <= -0.9)
						velocity.m_quad.v[1] *= -1;
					if(normal.GetX() >= 0.9 || normal.GetX() <= -0.9)
						velocity.m_quad.v[0] *= -1;
					if(normal.GetZ() >= 0.9 || normal.GetZ() <= -0.9)
						velocity.m_quad.v[2] *= -1;

					outVec = velocity;
				}
				else if(normal.dot(normal2) <= -0.9)
				{
					outVec = velocity;
				}
				else
				{
					if((normal.GetX() - normal2.GetX()) <= 0.01f && (normal.GetX() - normal2.GetX()) >= -0.01f)
					{
						/*
							[1, 0, 0 ]
							[0, c, -s]
							[0, s, c ]
						*/

						if(normal.GetZ() >= 0.1)
							rotAngle = 90;
						else if(normal.GetZ() <= -0.1)
							rotAngle = -90;
						if(normal.GetY() >= 0.1)
							rotAngle = -90;
						else if(normal.GetY() <= -0.1)
							rotAngle = 90;

						if(normal2.GetZ() <= -0.9 || normal2.GetY() <= -0.9)
							rotAngle *= -1;

						hkMatrix3 mat; 
						mat.setCols(hkVector4(1, 0, 0),
									hkVector4(0, hkMath::cos(HK_REAL_DEG_TO_RAD*rotAngle), -hkMath::sin(HK_REAL_DEG_TO_RAD*rotAngle)),
									hkVector4(0, hkMath::sin(HK_REAL_DEG_TO_RAD*rotAngle), hkMath::cos(HK_REAL_DEG_TO_RAD*rotAngle))
									);

						mat.multiplyVector(velocity, outVec);
					}
					else if((normal.GetY() - normal2.GetY()) <= 0.01f && (normal.GetY() - normal2.GetY()) >= -0.01f)
					{
						/*
							[c, 0, s ]
							[0, 1, 0 ]
							[-s, 0, c]
						*/

						if(normal.GetX() >= 0.1)
							rotAngle = 90;
						else if(normal.GetX() <= -0.1)
							rotAngle = -90;
						if(normal.GetZ() >= 0.1)
							rotAngle = -90;
						else if(normal.GetZ() <= -0.1)
							rotAngle = 90;

						if(normal2.GetX() <= -0.9 || normal2.GetZ() <= -0.9)
							rotAngle *= -1;

						hkMatrix3 mat;
						mat.setCols(hkVector4(hkMath::cos(HK_REAL_DEG_TO_RAD*rotAngle), 0, hkMath::sin(HK_REAL_DEG_TO_RAD*rotAngle)),
									hkVector4(0, 1, 0),
									hkVector4(-hkMath::sin(HK_REAL_DEG_TO_RAD*rotAngle), 0, hkMath::cos(HK_REAL_DEG_TO_RAD*rotAngle)));

						mat.multiplyVector(velocity, outVec);
					}
					else if((normal.GetZ() - normal2.GetZ()) <= 0.01f && (normal.GetZ() - normal2.GetZ()) >= -0.01f)
					{
						/*
							[c, -s, 0]
							[s, c, 0 ]
							[0, 0, 1 ]
						*/
						if(normal.GetX() >= 0.1)
							rotAngle = -90;
						else if(normal.GetX() <= -0.1)
							rotAngle = 90;
						if(normal.GetY() >= 0.1)
							rotAngle = 90;
						else if(normal.GetY() <= -0.1)
							rotAngle = -90;

						if(normal2.GetX() <= -0.9 || normal2.GetY() <= -0.9)
							rotAngle *= -1;

						hkMatrix3 mat;
						mat.setCols(hkVector4(hkMath::cos(HK_REAL_DEG_TO_RAD*rotAngle), -hkMath::sin(HK_REAL_DEG_TO_RAD*rotAngle), 0),
									hkVector4(hkMath::sin(HK_REAL_DEG_TO_RAD*rotAngle), hkMath::cos(HK_REAL_DEG_TO_RAD*rotAngle), 0),
									hkVector4(0, 0, 1));

						mat.multiplyVector(velocity, outVec);
					}
					else
						outVec = velocity;
				}
				if(outVec.length3() > 100.f)
				{
					outVec.normalize3();
					outVec.mul(100.f);
				}
				else
					outVec.mul4(1.1);
				RBb->setLinearVelocity(outVec);
				RBb->setPosition(tempPos);
				SoundEngine::getInstance()->getSoundSystem()->playSound(FMOD_CHANNEL_FREE, Portal::sPortEnter[RandomNumber::getRandomNumber(0,2)], false, &Portal::aPortals[0]->sChannel);

				//release gravgun
				if(Camera::getInstance()->isHeld())
					Camera::getInstance()->dropObject();
			}
			if(UDa == 1)
			{
				if(!left) return;
				Portal::aPortals[0]->pEvents->left = false;
				Vector temp = Portal::aPortals[0]->vPosition;
				Vector normal = Portal::aPortals[0]->vNormal;
				Vector normal2 = Portal::aPortals[1]->vNormal;
				normal.normalize();normal2.normalize();
				hkVector4 tempPos;if(RBb->getUserData() == BLACK)
				{
					tempPos.set(temp.GetX() + (normal.GetX()*7.f), temp.GetY() + (normal.GetY()*7.f), temp.GetZ() + (normal.GetZ()*7.f));
					Portal::aPortals[0]->pEvents->left = true;
				}
				else
				{
					tempPos.set(temp.GetX() + (normal.GetX()*1.1f), temp.GetY() + (normal.GetY()*1.1f), temp.GetZ() + (normal.GetZ()*1.1f));
				}
				hkVector4 velocity = RBb->getLinearVelocity();
				hkVector4 outVec;
				hkReal rotAngle = 0;
				if(normal.dot(normal2) >= 0.9)
				{
					if(normal.GetY() >= 0.9 || normal.GetY() <= -0.9)
						velocity.m_quad.v[1] *= -1;
					if(normal.GetX() >= 0.9 || normal.GetX() <= -0.9)
						velocity.m_quad.v[0] *= -1;
					if(normal.GetZ() >= 0.9 || normal.GetZ() <= -0.9)
						velocity.m_quad.v[2] *= -1;

					outVec = velocity;
				}
				else if(normal.dot(normal2) <= -0.9)
				{
					outVec = velocity;
				}
				else
				{
					if((normal.GetX() - normal2.GetX()) <= 0.01f && (normal.GetX() - normal2.GetX()) >= -0.01f)
					{
						/*
							[1, 0, 0 ]
							[0, c, -s]
							[0, s, c ]
						*/

						if(normal.GetZ() >= 0.1)
							rotAngle = 90;
						else if(normal.GetZ() <= -0.1)
							rotAngle = -90;
						if(normal.GetY() >= 0.1)
							rotAngle = -90;
						else if(normal.GetY() <= -0.1)
							rotAngle = 90;

						if(normal2.GetY() <= -0.9 || normal2.GetZ() <= -0.9)
							rotAngle *= -1;

						hkMatrix3 mat; 
						mat.setCols(hkVector4(1, 0, 0),
									hkVector4(0, hkMath::cos(HK_REAL_DEG_TO_RAD*rotAngle), -hkMath::sin(HK_REAL_DEG_TO_RAD*rotAngle)),
									hkVector4(0, hkMath::sin(HK_REAL_DEG_TO_RAD*rotAngle), hkMath::cos(HK_REAL_DEG_TO_RAD*rotAngle))
									);

						mat.multiplyVector(velocity, outVec);
					}
					else if((normal.GetY() - normal2.GetY()) <= 0.01f && (normal.GetY() - normal2.GetY()) >= -0.01f)
					{
						/*
							[c, 0, s ]
							[0, 1, 0 ]
							[-s, 0, c]
						*/

						if(normal.GetX() >= 0.1)
							rotAngle = 90;
						else if(normal.GetX() <= -0.1)
							rotAngle = -90;
						if(normal.GetZ() >= 0.1)
							rotAngle = -90;
						else if(normal.GetZ() <= -0.1)
							rotAngle = 90;

						if(normal2.GetX() <= -0.9 || normal2.GetZ() <= -0.9)
							rotAngle *= -1;

						hkMatrix3 mat;
						mat.setCols(hkVector4(hkMath::cos(HK_REAL_DEG_TO_RAD*rotAngle), 0, hkMath::sin(HK_REAL_DEG_TO_RAD*rotAngle)),
									hkVector4(0, 1, 0),
									hkVector4(-hkMath::sin(HK_REAL_DEG_TO_RAD*rotAngle), 0, hkMath::cos(HK_REAL_DEG_TO_RAD*rotAngle)));

						mat.multiplyVector(velocity, outVec);
					}
					else if((normal.GetZ() - normal2.GetZ()) <= 0.01f && (normal.GetZ() - normal2.GetZ()) >= -0.01f)
					{
						/*
							[c, -s, 0]
							[s, c, 0 ]
							[0, 0, 1 ]
						*/
						if(normal.GetX() >= 0.1)
							rotAngle = -90;
						else if(normal.GetX() <= -0.1)
							rotAngle = 90;
						if(normal.GetY() >= 0.1)
							rotAngle = 90;
						else if(normal.GetY() <= -0.1)
							rotAngle = -90;

						if(normal2.GetX() <= -0.9 || normal2.GetY() <= -0.9)
							rotAngle *= -1;

						hkMatrix3 mat;
						mat.setCols(hkVector4(hkMath::cos(HK_REAL_DEG_TO_RAD*rotAngle), -hkMath::sin(HK_REAL_DEG_TO_RAD*rotAngle), 0),
									hkVector4(hkMath::sin(HK_REAL_DEG_TO_RAD*rotAngle), hkMath::cos(HK_REAL_DEG_TO_RAD*rotAngle), 0),
									hkVector4(0, 0, 1));

						mat.multiplyVector(velocity, outVec);
					}
				}
				if(outVec.length3() > 100.f)
				{
					outVec.normalize3();
					outVec.mul(100.f);
				}
				else
					outVec.mul4(1.1);
				RBb->setLinearVelocity(outVec);
				RBb->setPosition(tempPos);
				SoundEngine::getInstance()->getSoundSystem()->playSound(FMOD_CHANNEL_FREE, Portal::sPortEnter[RandomNumber::getRandomNumber(0,2)], false, &Portal::aPortals[1]->sChannel);

				//release gravgun
				if(Camera::getInstance()->isHeld())
					Camera::getInstance()->dropObject();
			}
		}
	}
}

Portal::Portal(Vector position, Vector normal)
{
	pTex = NULL;
	vScale.SetValues(4.f, 1.f, 4.f);
	vRotation.SetValues(0.f, 0.f, 0.f, 0.f);
	vPosition = position;
	vNormal = normal;
	Vector temp = normal / 250.f;
	vPosition += temp;
	if(vNormal.GetY() >= 0.9)
		vRotation.SetValues(0.f, 0.f, 0.f, 0.f);
	else if(vNormal.GetY() <= -0.9)
		vRotation.SetValues(1.f, 0.f, 0.f, HK_REAL_DEG_TO_RAD*180);

	if(vNormal.GetX() >= 0.9)
		vRotation.SetValues(0.f, 0.f, 1.f, -HK_REAL_DEG_TO_RAD*90);
	else if(vNormal.GetX() <= -0.9)
		vRotation.SetValues(0.f, 0.f, 1.f, HK_REAL_DEG_TO_RAD*90);

	if(vNormal.GetZ() >= 0.9)
		vRotation.SetValues(1.f, 0.f, 0.f, HK_REAL_DEG_TO_RAD*90);
	else if(vNormal.GetZ() <= -0.9)
		vRotation.SetValues(1.f, 0.f, 0.f, -HK_REAL_DEG_TO_RAD*90);

	{
		hkpRigidBodyCinfo boxInfo;
		hkVector4 phantomSize(2.f,0.2f,2.f);
		hkpShape* boxShape = new hkpBoxShape(phantomSize, 0);
		boxInfo.m_motionType = hkpMotion::MOTION_FIXED;
		boxInfo.m_position.set(position.GetX(),position.GetY(),position.GetZ());
		if(vRotation.GetW() > 0.f)
		{
			hkVector4 temp(vRotation.GetX(), vRotation.GetY(), vRotation.GetZ());
			temp.normalize3();
			boxInfo.m_rotation.setAxisAngle(temp, vRotation.GetW());
		}

		pEvents = new PortalPhantom();
		hkpBvShape* bvShape = new hkpBvShape(boxShape, pEvents);
		boxShape->removeReference();
		pEvents->removeReference();

		boxInfo.m_shape = bvShape;

		pPhantom = new hkpRigidBody(boxInfo);
		bvShape->removeReference();

		DirectXRenderer::getInstance()->getHKWorld()->addEntity(pPhantom);
		pPhantom->removeReference();
	}

	FMOD_VECTOR vel;
	vel.x = vel.y = vel.z = 0;
	sChannel->set3DAttributes(&position.fmodGetVector(), &vel);
	sChannel->set3DMinMaxDistance(0.1f, 20.f);
}

void Portal::Render(void)
{
	DirectXRenderer::getInstance()->pushMatrix();
	DirectXRenderer::getInstance()->scale(vScale);
	DirectXRenderer::getInstance()->rotateAxis(vRotation, vRotation.GetW());
	DirectXRenderer::getInstance()->translate(vPosition);
	DirectXRenderer::getInstance()->setGameVertDecl();
	DirectXRenderer::getInstance()->getDevice()->SetTexture(0, pTex);
	DirectXRenderer::getInstance()->getDevice()->SetStreamSource(0, pVB, 0, sizeof(DirectXRenderer::VERTEX3D));
	DirectXRenderer::getInstance()->getDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	DirectXRenderer::getInstance()->popMatrix();
}

void Portal::Update(double dTime)
{

}

Portal::~Portal(void)
{
	if(pPhantom != NULL)
	{
		DirectXRenderer::getInstance()->getHKWorld()->lock();
		DirectXRenderer::getInstance()->getHKWorld()->removeEntity(pPhantom);
		DirectXRenderer::getInstance()->getHKWorld()->unlock();
		pPhantom = NULL;
	}
}