#ifndef __GAME_H_
#define __GAME_H_

#include "Scene.h"
#include "SceneNode.h"
#include "SceneNodePhysics.h"
#include "UIObject.h"
#include "Light.h"
#include "Levels.h"

class Game : public Scene
{
private:
	void Update(double dTime);
	void Render(void);
	SceneNode* gun;
public:
	Scene* level;
	Game(void);
	virtual ~Game(void);
};

#endif //__GAME_H_