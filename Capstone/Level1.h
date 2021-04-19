#ifndef __LEVEL_ONE_H_
#define __LEVEL_ONE_H_

#include "Basket.h"
#include "DirectXRenderer.h"
#include "Light.h"
#include "SceneNodePhysics.h"

class Level1 : public Scene
{
public:
	Light* light;

	Level1(void)
	{
		currentLevel = 1;

		light = new Light(Vector(0.f, 10.f, 0.f), WHITE, WHITE, WHITE, 10.f);

		Basket* basket = new Basket(Vector(-30.f, 2.1f, -30.f));
		basket->setUserData(WHITE);
		objToRender.push_back(basket);
	
		DebugCube* floor = new DebugCube(Vector(0.f, 0.f, 2.5f), 80.f, 80.f, 5.f);
		floor->SetTexture("concrete_modular_floor001c.rtf");
		floor->SetBump("concrete_modular_floor002_normal.rtf");
		floor->SetUVTiling(18.f, 18.f);

		DebugCube* roof = new DebugCube(Vector(0.f, 20.f, -2.5f), 80.f, 80.f, 5.f);
		roof->SetTexture("concrete_modular_ceiling001a.rtf");
		roof->SetBump("concrete_modular_ceiling001a_normal.rtf");
		roof->SetUVTiling(18.f, 18.f);

		DebugCube* wall1 = new DebugCube(Vector(37.5f, 10.f, 0.f), 5.f, 80.f, 20.f);
		wall1->SetTexture("concrete_modular_wall001a.rtf");
		wall1->SetBump("concrete_modular_wall001a_normal.rtf");
		wall1->SetUVTiling(16.f, 3.f);

		DebugCube* wall2 = new DebugCube(Vector(-37.5f, 10.f, 0.f), 5.f, 80.f, 20.f);
		wall2->SetTexture("concrete_modular_wall001a.rtf");
		wall2->SetBump("concrete_modular_wall001a_normal.rtf");
		wall2->SetUVTiling(16.f, 3.f);

		DebugCube* wall3 = new DebugCube(Vector(0.f, 10.f, 37.5f), 80.f, 5.f, 20.f);
		wall3->SetTexture("concrete_modular_wall001a.rtf");
		wall3->SetBump("concrete_modular_wall001a_normal.rtf");
		wall3->SetUVTiling(16.f, 3.f);

		DebugCube* wall4 = new DebugCube(Vector(0.f, 10.f, -37.5f), 80.f, 5.f, 20.f);
		wall4->SetTexture("concrete_modular_wall001a.rtf");
		wall4->SetBump("concrete_modular_wall001a_normal.rtf");
		wall4->SetUVTiling(16.f, 3.f);

		DebugCube* extra = new DebugCube(Vector(-30.f, 10.f, 5.f), 10.f, 20.f, 20.f);
		extra->SetTexture("metalwall048b.rtf");
		extra->SetBump("metalwall048b_normal.rtf");
		extra->SetPortalable(false);
		extra->SetUVTiling(5.f, 3.f);

		objToRender.push_back(wall1);
		objToRender.push_back(wall2);
		objToRender.push_back(wall3);
		objToRender.push_back(wall4);
		objToRender.push_back(extra);
		objToRender.push_back(roof);
		objToRender.push_back(floor);

		SceneNodePhysics* box = new SceneNodePhysics("Cube.rmf", 10.f, 15.f, 10.f, SceneNodePhysics::NODE_DYNAMIC, 200.f);
		box->setUserData(WHITE);
		objToRender.push_back(box);

		//DebugCube* glassWall = new DebugCube(Vector(0.f, 8.f, -25.f), 70.f, 1.5f, 13.f);
		//glassWall->SetTexture("Glass.png");
		//glassWall->setUserData(CYAN);
		//glassWall->SetPortalable(false);

		//objToRender.push_back(glassWall);
	}

	void Render(void)
	{
		RenderObjects();
	}

	void Update(double dTime)
	{
		light->Update();
		UpdateObjects(dTime/1000.f);
	}

	virtual ~Level1(void)
	{
		delete light;
	}
};

#endif