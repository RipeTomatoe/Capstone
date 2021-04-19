#ifndef __LIGHT_H_
#define __LIGHT_H_

#include "DirectXRenderer.h"
#include "Vector.h"
#include "Colour.h"

class Light
{
private:
	/*enum LIGHT_TYPE
	{
		LIGHT_POINT = 0x0,
		LIGHT_SPOT = 0x1,
		LIGHT_DIRECTIONAL = 0x2
	};*/

	//LIGHT_TYPE Type;
	float Range, Falloff, Attenuation0, Attenuation1, Attenuation2, Theta, Phi;
	Vector Direction, Position;
	Colour Diffuse, Specular, Ambient;
	static int lightCount;
public:
	Light(Vector pos, COLOUR_VALUE amb, COLOUR_VALUE diff, COLOUR_VALUE spec, float range=5.f);

	void move(Vector move)
	{
		Position = move;
	}

	virtual ~Light(void){lightCount--;}
	inline Vector getPosition(void) { return Position; }
	inline Vector getDirection(void) { return Direction; }
	inline Colour getDiffuse(void) { return Diffuse; }
	inline Colour getAmbient(void) { return Ambient; }
	void Update(void);
};

#endif // __LIGHT_H_