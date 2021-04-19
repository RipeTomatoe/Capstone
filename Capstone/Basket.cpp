#include "Basket.h"

Basket::Basket(Vector position)
{
	//initiate the 4 sides and bottom
	{
		//floor
		cubes[0] = new DebugCube((Vector(0.f,0.f,0.f) + position), 5.5f, 5.5f, 1.f);
		//side1
		cubes[1] = new DebugCube((Vector(0.5f, 1.f, -2.5f) + position), 5.f, 1.f, 5.f);
		//side2
		cubes[2] = new DebugCube((Vector(2.5f, 1.f, 0.5f) + position), 1.f, 5.f, 5.f);
		//side3
		cubes[3] = new DebugCube((Vector(-0.5f, 1.f, 2.5f) + position), 5.f, 1.f, 5.f);
		//side4
		cubes[4] = new DebugCube((Vector(-2.5f, 1.f, -0.5f) + position), 1.f, 5.f, 5.f);
		for(int i = 0; i < 5; i++)
		{
			cubes[i]->SetPortalable(false);
		}
	}

	//initiate the phantom
	{
		hkpRigidBodyCinfo boxInfo;
		hkVector4 phantomSize(2.f,0.5f,2.f);
		hkpShape* boxShape = new hkpBoxShape(phantomSize, 0);
		boxInfo.m_motionType = hkpMotion::MOTION_FIXED;
		boxInfo.m_position.set(0.f + position.GetX(),2.f + position.GetY(),0.f + position.GetZ());

		MyPhantomShape* myPhantom = new MyPhantomShape();
		hkpBvShape* bvShape = new hkpBvShape(boxShape, myPhantom);
		boxShape->removeReference();
		myPhantom->removeReference();

		boxInfo.m_shape = bvShape;

		phantom = new hkpRigidBody(boxInfo);
		bvShape->removeReference();

		DirectXRenderer::getInstance()->getHKWorld()->addEntity(phantom);
		phantom->removeReference();
	}
}

void Basket::Render(void)
{
	for(int i = 0; i < 5; ++i)
	{
		cubes[i]->Render();
	}
}

void Basket::Update(double dTime)
{
	for(int i = 0; i < 5; ++i)
	{
		cubes[i]->Update(dTime);
	}
}

void Basket::setTexture(const char* fileName)
{
	for(int i = 0; i < 5; ++i)
	{
		cubes[i]->SetTexture(fileName);
	}
}

void Basket::setUserData(ULONG data)
{
	userData = data;
	phantom->setUserData(userData);
}

Basket::~Basket(void)
{
	for(int i = 0; i < 5; ++i)
	{
		delete cubes[i];
		cubes[i] = NULL;
	}
}