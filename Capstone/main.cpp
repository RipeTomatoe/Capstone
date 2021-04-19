#include <Windows.h>
#include "DirectXRenderer.h"
#include "FirstMenu.h"
#include "Game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	FirstMenu* menu = new FirstMenu();
	//Game* game = new Game();
	DirectXRenderer::getInstance()->setActiveScene(menu);
	UINT temp;
	DirectXRenderer::getInstance()->run(&temp);
	DirectXRenderer::getInstance()->killHavok();
	return (int)temp;
}