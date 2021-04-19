#ifndef __FIRSTMENU_H_
#define __FIRSTMENU_H_

#include "Scene.h"
#include "Game.h"
#include "DirectXRenderer.h"
//#include "SoundEngine.h"
#include "UIObject.h"

class FirstMenu : public Scene
{
private:
	void Update(double dTime);
	void Render(void);
	LPD3DXFONT pFont;
	RECT FontPos;
	UIObject *pBGScreen, *pTitle;
public:
	FirstMenu(void);
	virtual ~FirstMenu(void);
};

#endif //__FIRSTMENU_H_