#include "GameOver.h"
#include "FirstMenu.h"

GameOver::GameOver(UINT score)
{
	UIObject* screen = new UIObject("scroll.jpg", DirectXRenderer::getInstance()->getWidth(), DirectXRenderer::getInstance()->getHeight(), 0, 0);

	objToRender.push_back(screen);

	D3DXFONT_DESC FontDesc = {28,0,400,0,false,DEFAULT_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_PITCH,"Comic Sans"};

	FontPos.top = (long)(DirectXRenderer::getInstance()->getHeight()/2.0);
	FontPos.left = 0;
	FontPos.right = (long)DirectXRenderer::getInstance()->getWidth();
	FontPos.bottom = (long)DirectXRenderer::getInstance()->getHeight();

	D3DXCreateFontIndirect(DirectXRenderer::getInstance()->getDevice(), &FontDesc, &pFont);
}

GameOver::~GameOver(void)
{
	SAFE_RELEASE(pFont);
}

void GameOver::Update(double dTime)
{
	UpdateObjects(dTime);

	if(DirectXRenderer::getInstance()->getInput()->isKeyDown(KEY_ENTER))
	{
		FirstMenu* menu = new FirstMenu();
		DirectXRenderer::getInstance()->setActiveScene(menu);
	}
}

void GameOver::Render(void)
{
	RenderObjects();

	char temp[63];

	sprintf_s(temp, "Game over! Press ENTER to continue.");

	pFont->DrawTextA(NULL,
					temp,
					-1,
					&FontPos,
					DT_CENTER,
					D3DCOLOR_XRGB(0,0,0));
}