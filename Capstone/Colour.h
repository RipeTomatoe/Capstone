#ifndef __COLOUR_H_
#define __COLOUR_H_

#include "DirectXRenderer.h"

enum COLOUR_VALUE
{
	WHITE		= 0xffffff,
	BLACK		= 0x000000,
	LIGHT_GREY	= 0x444444,
	DARK_GREY	= 0xbbbbbb,
	MEDIUM_GREY = 0x888888,
	RED			= 0xff0000,
	GREEN		= 0x00ff00,
	BLUE		= 0x0000ff,
	YELLOW		= 0xffff00,
	MAGENTA		= 0xff00ff,
	CYAN		= 0x00ffff
};

class Colour
{
private:
	BYTE r,g,b,a;

	/********************************************************************************************
	Summary:	[INTERNAL] Converts the hex code colour value to the RGB equivalent.
	Parameters: [in] cv - A colour value to convert.
	********************************************************************************************/
	void HexToRGB(COLOUR_VALUE cv)
	{
		r = (BYTE)((cv >> 16) & 0xFF);
		g = (BYTE)((cv >> 8) & 0xFF);
		b = (BYTE)((cv) & 0xFF);
	};
public:
	/********************************************************************************************
	Summary:	Default (empty) Constructor. Sets r, g, b to 0, and alpha to 255.
	Parameters: None.
	********************************************************************************************/
	Colour(void) { r=g=b=0;a=255; };

	/********************************************************************************************
	Summary:	Default Constructor.
	Parameters: [in] Red - sets red value.
				[in] Green - sets green value.
				[in] Blue - sets blue value.
	********************************************************************************************/
	Colour(BYTE Red, BYTE Green, BYTE Blue, BYTE Alpha = 255)
	{
		r = Red;
		g = Green;
		b = Blue;
		a = Alpha;
	}

	/********************************************************************************************
	Summary:	Default Constructor.
	Parameters: [in] rgb - an array of bytes representing the Red, Green and Blue
							colours respectively.
	********************************************************************************************/
	Colour(BYTE* rgb)
	{
		if(sizeof(rgb) > 3)
		{
			r = rgb[0];
			g = rgb[1];
			b = rgb[2];
			a = 255;
		}
		else r=g=b=a=255;
	}

	/********************************************************************************************
	Summary:	Default Constructor.
	Parameters: [in] cv - a predefined colour value. (Check COLOUR::COLOURVALUE)
	********************************************************************************************/
	Colour(COLOUR_VALUE cv)
	{
		r=g=b=0;
		a=255;
		HexToRGB(cv);
	}

	/********************************************************************************************
	Summary:	Returns the red byte.
	Parameters: None.
	Returns:	BYTE.
	********************************************************************************************/
	BYTE GetRedByte(void) { return r; }

	/********************************************************************************************
	Summary:	Returns the green byte.
	Parameters: None.
	Returns:	BYTE.
	********************************************************************************************/
	BYTE GetGreenByte(void) { return g; }

	/********************************************************************************************
	Summary:	Returns the blue byte.
	Parameters: None.
	Returns:	BYTE.
	********************************************************************************************/
	BYTE GetBlueByte(void) { return b; }

	/********************************************************************************************
	Summary:	Returns the red byte as a float.
	Parameters: None.
	Returns:	float.
	********************************************************************************************/
	float GetRedFloat(void) { return ((float)r / 255.0f); }

	/********************************************************************************************
	Summary:	Returns the green byte as a float.
	Parameters: None.
	Returns:	float.
	********************************************************************************************/
	float GetGreenFloat(void) { return ((float)g / 255.0f); }

	/********************************************************************************************
	Summary:	Returns the blue byte as a float.
	Parameters: None.
	Returns:	float.
	********************************************************************************************/
	float GetBlueFloat(void) { return ((float)b / 255.0f); }

	/********************************************************************************************
	Summary:	Sets the values of all bytes.
	Parameters: [in] Red - red byte.
				[in] Green - green byte.
				[in] Blue - blue byte.
	********************************************************************************************/
	void SetValues(BYTE Red, BYTE Green, BYTE Blue, BYTE Alpha = 255)
	{
		r = Red;
		g = Green;
		b = Blue;
		a = Alpha;
	}

	/********************************************************************************************
	Summary:	Returns the red byte as a float.
	Parameters: [in] cv - a predefined colour value (Check COLOUR::COLOURVALUE).
	********************************************************************************************/
	void SetValues(COLOUR_VALUE cv)
	{
		HexToRGB(cv);
	}

	const D3DXVECTOR4 dxGetVector4(void)
	{
		return D3DXVECTOR4(GetRedFloat(),GetGreenFloat(),GetBlueFloat(),(a/255.0f));
	}

	/********************************************************************************************
	Summary:	Default Deconstructor.
	Parameters: None.
	********************************************************************************************/
	virtual ~Colour(void){r=g=b=a=NULL;}
};

#endif