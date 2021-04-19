#ifndef __LEVEL_TWO_H_
#define __LEVEL_TWO_H_

#include "Scene.h"
#include "Light.h"

class Level2 : public Scene
{
public:
	Light* light;
	int dir;
	Level2(void)
	{
		/*
		 ______________
		|			   |
		|		s      |					________
		|              |				   |	 E  |
		|			   |___________________|		|
		|											|
		|											|
		|											|
		|						 b  				|
		|___________________________________________|
		*/

		currentLevel = 2;
		dir = 1;

		light = new Light(Vector(32.f, 22.f, 38.f), WHITE, WHITE, WHITE, 35.f);

		Basket* basket = new Basket(Vector(19.f, 19.7f, 80.f));
		basket->setUserData(WHITE);
		objToRender.push_back(basket);
		
		DebugCube* roof = new DebugCube(Vector(23.f, 42.5f, 38.f), 56.f, 86.f, 5.f);
		DebugCube* floor = new DebugCube(Vector(23.f, -2.5f, 38.f), 56.f, 86.f, 5.f);

		floor->SetTexture("concrete_modular_floor001c.rtf");
		floor->SetBump("concrete_modular_floor002_normal.rtf");
		floor->SetUVTiling(13.f, 19.f);

		roof->SetTexture("metal_modular_floor002.rtf");
		roof->SetBump("metal_modular_floor001_normal.rtf");
		roof->SetPortalable(false);
		roof->SetUVTiling(4.f, 6.f);

		DebugCube* wall1 = new DebugCube(Vector(-7.5f, 20.f, 10.f), 5.f, 30.f, 40.f);
		wall1->SetTexture("metalwall048b.rtf");
		wall1->SetBump("metalwall048b_normal.rtf");
		wall1->SetPortalable(false);
		wall1->SetUVTiling(7.f, 8.f);

		DebugCube* wall2 = new DebugCube(Vector(10.f, 20.f, 30.f), 40.f, 40.f, 40.f);
		wall2->SetTexture("metalwall048b.rtf");
		wall2->SetBump("metalwall048b_normal.rtf");
		wall2->SetPortalable(false);
		wall2->SetUVTiling(9.f, 8.f);
		
		DebugCube* wall3 = new DebugCube(Vector(-2.5f, 20.f, 65.f), 32.f, 32.f, 40.f);
		wall3->SetTexture("metalwall048b.rtf");
		wall3->SetBump("metalwall048b_normal.rtf");
		wall3->SetPortalable(false);
		wall3->SetUVTiling(8.f, 8.f);

		DebugCube* wall4 = new DebugCube(Vector(32.f, 20.f, 83.5f), 38.f, 5.f, 40.f);
		wall4->SetTexture("metalwall048b.rtf");
		wall4->SetBump("metalwall048b_normal.rtf");
		wall4->SetPortalable(false);
		wall4->SetUVTiling(8.f, 8.f);

		DebugCube* wall5 = new DebugCube(Vector(53.5f, 20.f, 38.f), 5.f, 86.f, 40.f);
		wall5->SetTexture("metalwall048b.rtf");
		wall5->SetBump("metalwall048b_normal.rtf");
		wall5->SetPortalable(false);
		wall5->SetUVTiling(15.f, 8.f);

		DebugCube* wall6 = new DebugCube(Vector(23.f, 20.f, -7.5f), 56.f, 5.f, 40.f);
		wall6->SetTexture("metalwall048b.rtf");
		wall6->SetBump("metalwall048b_normal.rtf");
		wall6->SetPortalable(false);
		wall6->SetUVTiling(10.f, 8.f);

		DebugCube* floor2 = new DebugCube(Vector(38.f, -2.5f, 34.8f), 30.f, 30.f, 30.f);
		floor2->SetTexture("metalwall048b.rtf");
		floor2->SetBump("metalwall048b_normal.rtf");
		floor2->SetPortalable(false);
		floor2->SetUVTiling(8.f, 8.f);

		DebugCube* floor3 = new DebugCube(Vector(10.f, 10.f, 65.f), 32.f, 32.f, 20.f);
		floor3->SetTexture("metalwall048b.rtf");
		floor3->SetBump("metalwall048b_normal.rtf");
		floor3->SetPortalable(false);
		floor3->SetUVTiling(8.f, 8.f);

		objToRender.push_back(floor3);
		objToRender.push_back(floor2);
		objToRender.push_back(wall6);
		objToRender.push_back(wall5);
		objToRender.push_back(wall4);
		objToRender.push_back(wall3);
		objToRender.push_back(wall2);
		objToRender.push_back(wall1);
		objToRender.push_back(floor);
		objToRender.push_back(roof);

		DebugCube* outlet = new DebugCube(Vector(42.f, 30.f, -7.f), 4.f, 5.f, 5.f);
		outlet->SetTexture("concrete_modular_wall001a.rtf");
		outlet->SetBump("concrete_modular_wall001a_normal.rtf");
		objToRender.push_back(outlet);

		DebugCube* outlet2 = new DebugCube(Vector(53.f,30.f, 65.f), 5.f, 4.f, 5.f);
		outlet2->SetTexture("concrete_modular_wall001a.rtf");
		outlet2->SetBump("concrete_modular_wall001a_normal.rtf");
		objToRender.push_back(outlet2);

		SceneNodePhysics* box = new SceneNodePhysics("Cube.rmf", 3.f, 15.f, 3.f, SceneNodePhysics::NODE_DYNAMIC, 200.f);
		box->setUserData(WHITE);
		objToRender.push_back(box);
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

	virtual ~Level2(void)
	{
		delete light;
	}
};

#endif