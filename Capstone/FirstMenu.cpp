#include "FirstMenu.h"

FirstMenu::FirstMenu(void)
{
	is3D = false;
	pBGScreen = new UIObject("scroll.rtf", DirectXRenderer::getInstance()->getWidth(), DirectXRenderer::getInstance()->getHeight(), 0, 0);

	objToRender.push_back(pBGScreen);

	{
		D3DXFONT_DESC FontDesc = {28,0,400,0,false,DEFAULT_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_PITCH,"Comic Sans"};

		FontPos.top = (long)(DirectXRenderer::getInstance()->getHeight()/2.0);
		FontPos.left = 0;
		FontPos.right = (long)DirectXRenderer::getInstance()->getWidth();
		FontPos.bottom = (long)DirectXRenderer::getInstance()->getHeight();

		D3DXCreateFontIndirect(DirectXRenderer::getInstance()->getDevice(), &FontDesc, &pFont);
	}

	//SoundEngine::getInstance()->playMusic("chiptune1.mp3", true, 0.5f);

	isLoading=false;
}

FirstMenu::~FirstMenu(void)
{
	SoundEngine::getInstance()->stopSounds();
	SAFE_RELEASE(pFont);
}

void FirstMenu::Update(double dTime)
{
	UpdateObjects(dTime);

	if(DirectXRenderer::getInstance()->getInput()->isKeyDown(KEY_ESC))
	{
		DirectXRenderer::getInstance()->quitProgram(0);
	}

	if(DirectXRenderer::getInstance()->getInput()->isKeyDown(KEY_ENTER))
	{
		Game* game = new Game();
		DirectXRenderer::getInstance()->setActiveScene(game);
	}
}

void FirstMenu::Render(void)
{
	RenderObjects();

	pFont->DrawTextA(NULL,
					"Press ENTER to Play!",
					-1,
					&FontPos,
					DT_CENTER,
					D3DCOLOR_XRGB(0,0,0));
}