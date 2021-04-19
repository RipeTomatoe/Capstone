#include "Game.h"
#include "Camera.h"
#include "FirstMenu.h"
#include "Enemy.h"
#include "Portal.h"

Game::Game(void)
{
	DirectXRenderer::getInstance()->getHKWorld()->lock();
	is3D = true;
	gun = NULL;

	Camera::reset();
	
	UINT16 tempWidth;
	tempWidth = DirectXRenderer::getInstance()->getWidth()/150;
	
	UIObject* crosshair = new UIObject("Yellow.rtf", tempWidth, tempWidth, (DirectXRenderer::getInstance()->getWidth()/2), (DirectXRenderer::getInstance()->getHeight()/2));
	gun = new SceneNode("PortalGun.rmf", 10.f, 15.f, 10.f);
	gun->setScale(0.5f);
	Camera::getInstance()->setNode(gun);
	UIToRender.push_back(crosshair);
	UIToRender.push_front(gun);	

	Portal::aPortals[0] = NULL;
	Portal::aPortals[1] = NULL;

	level = new Level1();

	isLoading=false;
	DirectXRenderer::getInstance()->getHKWorld()->unlock();
}

Game::~Game(void)
{
	delete level;
}

void Game::Update(double dTime)
{
	if(dTime >= 100)
		dTime = 16.7;
	if(currentLevel != level->currentLevel)
	{
		if(currentLevel == 2)
		{
			delete level;
			Camera::reset();
			if(gun != NULL)
				Camera::getInstance()->setNode(gun);
			Portal::reset();
			level = new Level2();
		}
	}

	Camera::getInstance()->update(dTime);
	DirectXRenderer::getInstance()->getHKWorld()->stepDeltaTime((hkReal)(dTime/1000.0));
	UpdateObjects(dTime/1000.0);

	level->Update(dTime);

	if(DirectXRenderer::getInstance()->getInput()->isKeyDown(KEY_F12))
		DirectXRenderer::getInstance()->screenshot();

	if(DirectXRenderer::getInstance()->getInput()->isKeyDown(KEY_ESC))
	{
		FirstMenu* menu = new FirstMenu();
		DirectXRenderer::getInstance()->setActiveScene(menu);
	}
}

void Game::Render(void)
{
	Camera::getInstance()->render();
	RenderObjects();
	level->Render();
	RenderUI();
}