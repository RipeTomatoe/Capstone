#ifndef __VECTOR_H_
#define __VECTOR_H_

#include "FMOD\Win32\fmod.hpp"
#include "assimp\include\assimp.h"
#include <d3dx9.h>
#include <d3d9.h>

class Vector
{
protected:
	float x, y, z, w;
public:
	Vector(void);
	Vector(float _x, float _y, float _z);
	Vector(float* fv);
	void SetX(float _x);
	void SetY(float _y);
	void SetZ(float _z);
	void SetValues(float _x, float _y, float _z, float _w=0.f);
	void SetValues(float* fv);
	const float GetX(void);
	const float GetY(void);
	const float GetZ(void);
	const float GetW(void);
	Vector GetNormal(void);
	void normalize(void);
	void negate(void);
	float dot(const Vector &other);
	Vector cross(const Vector &other);
	float magnitude(void);
	Vector operator+=(const Vector &other);
	Vector operator+(const Vector &other);
	Vector operator-=(const Vector &other);
	Vector operator-(const Vector &other);
	Vector operator/(const float &scalar);
	Vector operator*(const float &scalar);
	Vector operator*=(const float &scalar);
	virtual ~Vector(void);
	const FMOD_VECTOR fmodGetVector(void);
	const aiVector3D aiGetVector(void);
	const D3DXVECTOR3 dxGetVector(void);
	const D3DXVECTOR4 dxGetVector4(void);
};

#endif