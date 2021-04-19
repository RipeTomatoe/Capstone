#ifndef __GAMEOVER_H_
#define __GAMEOVER_H_

#include "Scene.h"
#include "DirectXRenderer.h"

class GameOver : public Scene
{
private:
	void Update(double dTime);
	void Render(void);
	LPD3DXFONT pFont;
	RECT FontPos;
public:
	GameOver(UINT score);
	virtual ~GameOver(void);
};

#endif