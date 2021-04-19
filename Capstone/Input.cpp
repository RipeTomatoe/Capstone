#include "Input.h"


Input::Input(TYPE type)
{
	currentInput = type;
	for(USHORT i = 0; i < 256; i++)
		keyCode[i] = false;
	for(USHORT i = 0; i < 256; i++)
		lastFrame[i] = false;

	lastLeft = lastRight = lastMiddle = false;

	if(currentInput == INPUT_DI)
	{
		pDIObject = NULL;

		if(!initDirectInput()) 
		{
			LogManager::getInstance()->trace("[ERROR] DirectInput initiliazation failed! Attemping to use RAW instead...");
			currentInput = INPUT_RAW;
			if(!initRAWInput())
			{
				LogManager::getInstance()->trace("[ERROR] RAW Input initialization failed!");
				LogManager::getInstance()->trace("[ERROR] No input methods could be initialized!");
				DirectXRenderer::getInstance()->quitProgram(1);
			}
		}
	}
	else if(currentInput == INPUT_RAW)
	{
		if(!initRAWInput()) 
		{
			LogManager::getInstance()->trace("[ERROR] RAW Input initiliazation failed! Attempting to use DirectInput instead...");
			currentInput = INPUT_DI;
			if(!initDirectInput())
			{
				LogManager::getInstance()->trace("[ERROR] DirectInput initialization failed!");
				LogManager::getInstance()->trace("[ERROR] No input methods could be initialized!");
				DirectXRenderer::getInstance()->quitProgram(1);
			}
		}
	}
	mouseState.x = mouseState.y = 0;
}

bool Input::initRAWInput(void)
{
	RAWINPUTDEVICE rid[2];

	rid[0].usUsagePage = 0x01;
	rid[0].usUsage = 0x02; //mouse
	rid[0].dwFlags = 0;
	rid[0].hwndTarget = 0;

	rid[1].usUsagePage = 0x01;
	rid[1].usUsage = 0x06; //keyboard
	rid[1].dwFlags = RIDEV_NOLEGACY;
	rid[1].hwndTarget = 0;

	if(RegisterRawInputDevices(rid, 2, sizeof(rid[0])) == FALSE)
	{
		LogManager::getInstance()->trace("[ERROR] Raw Input could not be initialized (%u)", GetLastError());
		return false;
	}

	return true;
}

bool Input::initDirectInput(void)
{
	LogManager::getInstance()->trace("Initializing DirectInput...");

	if(FAILED(DirectInput8Create(	GetModuleHandle(NULL),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&pDIObject,
		NULL)))
	{
		return false;
	}

	//KEYBOARD

	if(FAILED(pDIObject->CreateDevice(GUID_SysKeyboard,
		&pDIKeyboard,
		NULL)))
	{
		LogManager::getInstance()->trace("[ERROR] DirectInput could not create the keyboard device!");
		return false;
	}

	if(FAILED(pDIKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		LogManager::getInstance()->trace("[ERROR] DirectInput could not set the correct data format for the keyboard.");
		return false;
	}

	if(FAILED(pDIKeyboard->SetCooperativeLevel(NULL, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
	{
		LogManager::getInstance()->trace("[ERROR] DirectInput could not set the correct cooperative level for the keyboard.");
		return false;
	}

	if(FAILED(pDIKeyboard->Acquire()))
	{
		LogManager::getInstance()->trace("[ERROR] DirectInput could not acquire the keyboard.");
		return false;
	}

	//MOUSE

	if(FAILED(pDIObject->CreateDevice(GUID_SysMouse,
		&pDIMouse, NULL)))
	{
		LogManager::getInstance()->trace("[ERROR] DirectInput could not create the mouse device!");
		return false;
	}

	if(FAILED(pDIMouse->SetDataFormat(&c_dfDIMouse)))
	{
		LogManager::getInstance()->trace("[ERROR] DirectInput could not set the mouse data format.");
		return false;
	}

	if(FAILED(pDIMouse->SetCooperativeLevel(NULL, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
	{
		LogManager::getInstance()->trace("[ERROR] DirectInput could not cset the cooperative level for the mouse.");
		return false;
	}

	if(FAILED(pDIMouse->Acquire()))
	{
		LogManager::getInstance()->trace("[ERROR] DirectInput could not acquire the mouse.");
		return false;
	}

	LogManager::getInstance()->trace("DirectInput successfully initialized!");

	return true;
}

bool Input::Update(LPARAM lParam)
{
	if(currentInput == INPUT_DI)
	{
		char temp[256];
		if(FAILED(pDIKeyboard->GetDeviceState(sizeof(temp), (LPVOID)&temp)))
		{
			LogManager::getInstance()->trace("[ERROR] DirectInput could not acquire the keyboard state.");
			return false;
		}
		for(USHORT i = 0; i < 256; i++)
		{
			if(temp[i] & 0x80)
				keyCode[i] = true;
			else keyCode[i] = false;
		}

		DIMOUSESTATE mouseTemp;

		if(DIERR_INPUTLOST == pDIMouse->GetDeviceState(sizeof(mouseTemp), (LPVOID)&mouseTemp))
			pDIMouse->Acquire();

		if((mouseTemp.rgbButtons[0] & 0x80) && lastLeft)
			mouseState.left = false;
		else if((mouseTemp.rgbButtons[0] & 0x80) && !lastLeft)
		{
			lastLeft = true;
			mouseState.left = true;
		}
		else
		{
			lastLeft = false;
			mouseState.left = false;
		}

		if((mouseTemp.rgbButtons[1] & 0x80) && lastRight)
			mouseState.right = false;
		else if((mouseTemp.rgbButtons[1] & 0x80) && !lastRight)
		{
			lastRight = true;
			mouseState.right = true;
		}
		else
		{
			lastRight = false;
			mouseState.right = false;
		}

		if((mouseTemp.rgbButtons[2] & 0x80) && lastMiddle)
			mouseState.middle = false;
		else if((mouseTemp.rgbButtons[2] & 0x80) && !lastMiddle)
		{
			lastMiddle = true;
			mouseState.middle = true;
		}
		else
		{
			lastMiddle = false;
			mouseState.middle = false;
		}
		mouseState.x += mouseTemp.lX;
		mouseState.y += mouseTemp.lY;
	}
	else if(currentInput == INPUT_RAW)
	{
		if(lParam == NULL) return false;

		UINT dwSize;

		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
		LPBYTE lpb = new BYTE[dwSize];
		if(lpb == NULL)
			return 0;

		if(GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
			LogManager::getInstance()->trace("[WARNING] GetRawInputData did not return the correct size");

		RAWINPUT* raw = (RAWINPUT*)lpb;

		if(raw->header.dwType == RIM_TYPEKEYBOARD)
		{
			//keyboard
			//getInstance()->keyCode = raw->data.keyboard.VKey;
			if(raw->data.keyboard.Message == WM_KEYDOWN)
				keyCode[raw->data.keyboard.VKey] = true;
			else
				keyCode[raw->data.keyboard.VKey] = false;
		}
		else if(raw->header.dwType == RIM_TYPEMOUSE)
		{
			//mouse
			mouseState.x += raw->data.mouse.lLastX;
			mouseState.y += raw->data.mouse.lLastY;

			if(raw->data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_DOWN)
				mouseState.left = true;

			if(raw->data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_DOWN)
				mouseState.right = true;

			if(raw->data.mouse.ulButtons & RI_MOUSE_MIDDLE_BUTTON_DOWN)
				mouseState.middle = true;
		}

		delete[] lpb;
	}
	return true;
}

USHORT Input::keyConverter(USHORT key)
{
	if(currentInput == INPUT_RAW)
	{
		USHORT rawKey=0;
		switch(key)
		{
			case KEY_A:
				rawKey='A';
				break;
			case KEY_B:
				rawKey='B';
				break;
			case KEY_C:
				rawKey='C';
				break;
			case KEY_D:
				rawKey='D';
				break;
			case KEY_E:
				rawKey='E';
				break;
			case KEY_F:
				rawKey='F';
				break;
			case KEY_G:
				rawKey='G';
				break;
			case KEY_H:
				rawKey='H';
				break;
			case KEY_I:
				rawKey='I';
				break;
			case KEY_J:
				rawKey='J';
				break;
			case KEY_K:
				rawKey='K';
				break;
			case KEY_L:
				rawKey='L';
				break;
			case KEY_M:
				rawKey='M';
				break;
			case KEY_N:
				rawKey='N';
				break;
			case KEY_O:
				rawKey='O';
				break;
			case KEY_P:
				rawKey='P';
				break;
			case KEY_Q:
				rawKey='Q';
				break;
			case KEY_R:
				rawKey='R';
				break;
			case KEY_S:
				rawKey='S';
				break;
			case KEY_T:
				rawKey='T';
				break;
			case KEY_U:
				rawKey='U';
				break;
			case KEY_V:
				rawKey='V';
				break;
			case KEY_W:
				rawKey='W';
				break;
			case KEY_X:
				rawKey='X';
				break;
			case KEY_Y:
				rawKey='Y';
				break;
			case KEY_Z:
				rawKey='Z';
				break;
			case KEY_1:
				rawKey='1';
				break;
			case KEY_2:
				rawKey='2';
				break;
			case KEY_3:
				rawKey='3';
				break;
			case KEY_4:
				rawKey='4';
				break;
			case KEY_5:
				rawKey='5';
				break;
			case KEY_6:
				rawKey='6';
				break;
			case KEY_7:
				rawKey='7';
				break;
			case KEY_8:
				rawKey='8';
				break;
			case KEY_9:
				rawKey='9';
				break;
			case KEY_0:
				rawKey='0';
				break;
			case KEY_F1:
				rawKey=VK_F1;
				break;
			case KEY_F2:
				rawKey=VK_F2;
				break;
			case KEY_F3:
				rawKey=VK_F3;
				break;
			case KEY_F4:
				rawKey=VK_F4;
				break;
			case KEY_F5:
				rawKey=VK_F5;
				break;
			case KEY_F6:
				rawKey=VK_F6;
				break;
			case KEY_F7:
				rawKey=VK_F7;
				break;
			case KEY_F8:
				rawKey=VK_F8;
				break;
			case KEY_F9:
				rawKey=VK_F9;
				break;
			case KEY_F10:
				rawKey=VK_F10;
				break;
			case KEY_F11:
				rawKey=VK_F11;
				break;
			case KEY_F12:
				rawKey=VK_F12;
				break;
			case KEY_SPACE:
				rawKey=VK_SPACE;
				break;
			case KEY_ENTER:
				rawKey=VK_RETURN;
				break;
			case KEY_ESC:
				rawKey=VK_ESCAPE;
				break;
			case KEY_UP:
				rawKey=VK_UP;
				break;
			case KEY_DOWN:
				rawKey=VK_DOWN;
				break;
			case KEY_LEFT:
				rawKey=VK_LEFT;
				break;
			case KEY_RIGHT:
				rawKey=VK_RIGHT;
				break;
			default:
				rawKey=0;
		}
		return rawKey;
	}
	else if(currentInput == INPUT_DI)
	{
		USHORT DIKey=0;
		switch(key)
		{
			case KEY_A:
				DIKey=DIK_A;
				break;
			case KEY_B:
				DIKey=DIK_B;
				break;
			case KEY_C:
				DIKey=DIK_C;
				break;
			case KEY_D:
				DIKey=DIK_D;
				break;
			case KEY_E:
				DIKey=DIK_E;
				break;
			case KEY_F:
				DIKey=DIK_F;
				break;
			case KEY_G:
				DIKey=DIK_G;
				break;
			case KEY_H:
				DIKey=DIK_H;
				break;
			case KEY_I:
				DIKey=DIK_I;
				break;
			case KEY_J:
				DIKey=DIK_J;
				break;
			case KEY_K:
				DIKey=DIK_K;
				break;
			case KEY_L:
				DIKey=DIK_L;
				break;
			case KEY_M:
				DIKey=DIK_M;
				break;
			case KEY_N:
				DIKey=DIK_N;
				break;
			case KEY_O:
				DIKey=DIK_O;
				break;
			case KEY_P:
				DIKey=DIK_P;
				break;
			case KEY_Q:
				DIKey=DIK_Q;
				break;
			case KEY_R:
				DIKey=DIK_R;
				break;
			case KEY_S:
				DIKey=DIK_S;
				break;
			case KEY_T:
				DIKey=DIK_T;
				break;
			case KEY_U:
				DIKey=DIK_U;
				break;
			case KEY_V:
				DIKey=DIK_V;
				break;
			case KEY_W:
				DIKey=DIK_W;
				break;
			case KEY_X:
				DIKey=DIK_X;
				break;
			case KEY_Y:
				DIKey=DIK_Y;
				break;
			case KEY_Z:
				DIKey=DIK_Z;
				break;
			case KEY_1:
				DIKey=DIK_1;
				break;
			case KEY_2:
				DIKey=DIK_2;
				break;
			case KEY_3:
				DIKey=DIK_3;
				break;
			case KEY_4:
				DIKey=DIK_4;
				break;
			case KEY_5:
				DIKey=DIK_5;
				break;
			case KEY_6:
				DIKey=DIK_6;
				break;
			case KEY_7:
				DIKey=DIK_7;
				break;
			case KEY_8:
				DIKey=DIK_8;
				break;
			case KEY_9:
				DIKey=DIK_9;
				break;
			case KEY_0:
				DIKey=DIK_0;
				break;
			case KEY_F1:
				DIKey=DIK_F1;
				break;
			case KEY_F2:
				DIKey=DIK_F2;
				break;
			case KEY_F3:
				DIKey=DIK_F3;
				break;
			case KEY_F4:
				DIKey=DIK_F4;
				break;
			case KEY_F5:
				DIKey=DIK_F5;
				break;
			case KEY_F6:
				DIKey=DIK_F6;
				break;
			case KEY_F7:
				DIKey=DIK_F7;
				break;
			case KEY_F8:
				DIKey=DIK_F8;
				break;
			case KEY_F9:
				DIKey=DIK_F9;
				break;
			case KEY_F10:
				DIKey=DIK_F10;
				break;
			case KEY_F11:
				DIKey=DIK_F11;
				break;
			case KEY_F12:
				DIKey=DIK_F12;
				break;
			case KEY_SPACE:
				DIKey=DIK_SPACE;
				break;
			case KEY_ENTER:
				DIKey=DIK_RETURN;
				break;
			case KEY_ESC:
				DIKey=DIK_ESCAPE;
				break;
			case KEY_UP:
				DIKey=DIK_UP;
				break;
			case KEY_DOWN:
				DIKey=DIK_DOWN;
				break;
			case KEY_LEFT:
				DIKey=DIK_LEFT;
				break;
			case KEY_RIGHT:
				DIKey=DIK_RIGHT;
				break;
			default:
				DIKey=0;
		}
		return DIKey;
	}
	return 257;
}

bool Input::isKeyDown(USHORT key)
{
	USHORT temp = keyConverter(key);
	if(lastFrame[temp] && keyCode[temp])
		return false;
	else if(!(lastFrame[temp]) && keyCode[temp])
	{
		lastFrame[temp]=true;
		return true;
	}
	else
	{
		lastFrame[temp]=false;
		return false;
	}
}

bool Input::isKeyHeld(USHORT key)
{
	USHORT temp = keyConverter(key);
	return keyCode[temp];
}

Input::~Input(void)
{
	if(pDIKeyboard!=NULL)
	{
		pDIKeyboard->Unacquire();
		pDIKeyboard->Release();
		pDIKeyboard = NULL;
	}
	SAFE_RELEASE(pDIObject);
}