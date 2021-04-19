#ifndef __INPUT_H_
#define __INPUT_H_

#include "DirectXRenderer.h"

enum KEYS
{
	KEY_A=1,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_0,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_SPACE,
	KEY_ENTER,
	KEY_ESC,
	KEY_TILDE,
	KEY_MINUS,
	KEY_PLUS,
	KEY_CURLYLEFT,
	KEY_CURLYRIGHT,
	KEY_BACKSLASH,
	KEY_COLON,
	KEY_QUOTE,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_FORWARDSLASH,
	KEY_TAB,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_RIGHTCONTROL,
	KEY_LEFTCONTROL,
	KEY_RIGHTALT,
	KEY_LEFTALT,
	KEY_RIGHTSHIFT,
	KEY_LEFTSHIFT
};

class Input
{
public:
	enum TYPE
	{
		INPUT_DI = 0,
		INPUT_RAW
	};
private:
	struct MOUSESTATE
	{
		long x,y;
		bool left,middle,right;
		bool b1,b2,b3,b4,b5;
		bool wheel;
	};
	LPDIRECTINPUT8 pDIObject;
	LPDIRECTINPUTDEVICE8 pDIKeyboard;
	LPDIRECTINPUTDEVICE8 pDIMouse;
	TYPE currentInput;
	bool keyCode[256];
	bool lastFrame[256];
	bool lastLeft, lastRight, lastMiddle;
	MOUSESTATE mouseState;
	USHORT keyConverter(USHORT key);
public:
	Input(TYPE type);
	virtual ~Input(void);
	//DI
	bool initDirectInput(void);
	//RAW
	bool initRAWInput(void);
	//General
	bool Update(LPARAM lParam=NULL);
	void Reset(void)
	{
		if(currentInput == INPUT_RAW)
		{
			mouseState.middle = false;
			mouseState.left = false;
			mouseState.right = false;
		}
		mouseState.x = 0;
		mouseState.y = 0;
	}
	bool isKeyDown(USHORT key);
	bool isKeyHeld(USHORT key);
	MOUSESTATE getMouseState(void){return mouseState;}
};

#endif