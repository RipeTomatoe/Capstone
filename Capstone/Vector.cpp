#include "Vector.h"

Vector::Vector(void)
{
	x = y = z = w = 0.f;
}

Vector::Vector(float _x, float _y, float _z)
{
	w = 0.f;
	x = _x;
	y = _y;
	z = _z;
}

Vector::Vector(float* fv)
{
	if(sizeof(fv) >= (sizeof(float) * 3))
	{
		x = fv[0];
		y = fv[1];
		z = fv[2];
	}
	else
		x = y = z = 0;
}

void Vector::SetX(float _x)
{
	x = _x;
}

void Vector::SetY(float _y)
{
	y = _y;
}

void Vector::SetZ(float _z)
{
	z = _z;
}

void Vector::SetValues(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

void Vector::SetValues(float* fv)
{
	if(sizeof(fv) >= (sizeof(float) * 3))
	{
		x = fv[0];
		y = fv[1];
		z = fv[2];
	}
	else
		x = y = z = 0;
}

const float Vector::GetX(void)
{
	return x;
}

const float Vector::GetY(void)
{
	return y;
}

const float Vector::GetZ(void)
{
	return z;
}

const float Vector::GetW(void)
{
	return w;
}

Vector Vector::GetNormal(void)
{
	Vector temp = *this;
	float mag = this->magnitude();
	temp.SetValues(this->x / mag, this->y / mag, this->z / mag);
	return temp;
}

void Vector::normalize(void)
{
	float mag = this->magnitude();
	this->x /= mag;
	this->y /= mag;
	this->z /= mag;
}

void Vector::negate(void)
{
	x *= -1;
	y *= -1;
	z *= -1;
	w *= -1;
}

float Vector::dot(const Vector &other)
{
	return this->x * other.x + this->y * other.y + this->z * other.z;
}

Vector Vector::cross(const Vector &other)
{
	Vector temp((this->y * other.z - this->z * other.y),
		(this->z * other.x - this->x * other.z),
		(this->x * other.y - this->y * other.x));
	return temp;
}

float Vector::magnitude(void)
{
	Vector temp(x, y, z);
	return sqrt(this->dot(temp));
}

Vector Vector::operator+=(const Vector &other)
{
	this->x += other.x;
	this->y += other.y;
	this->z += other.z;
	return *this;
}

Vector Vector::operator+(const Vector &other)
{
	Vector temp;
	temp.SetX(this->x + other.x);
	temp.SetY(this->y + other.y);
	temp.SetZ(this->z + other.z);
	return temp;
}

Vector Vector::operator-=(const Vector &other)
{
	this->x -= other.x;
	this->y -= other.y;
	this->z -= other.z;
	return *this;
}

Vector Vector::operator-(const Vector &other)
{
	Vector temp;
	temp.SetX(this->x - other.x);
	temp.SetY(this->y - other.y);
	temp.SetZ(this->z - other.z);
	return temp;
}

Vector Vector::operator/(const float &scalar)
{
	Vector temp;
	temp.SetX(this->x / scalar);
	temp.SetY(this->y / scalar);
	temp.SetZ(this->z / scalar);
	return temp;
}

Vector Vector::operator*(const float &scalar)
{
	Vector temp;
	temp.SetX(this->x * scalar);
	temp.SetY(this->y * scalar);
	temp.SetZ(this->z * scalar);
	return temp;
}

Vector Vector::operator*=(const float &scalar)
{
	this->x *= scalar;
	this->y *= scalar;
	this->z *= scalar;
	return *this;
}

const FMOD_VECTOR Vector::fmodGetVector(void)
{
	FMOD_VECTOR temp = {x, y, z};
	return temp;
}

const aiVector3D Vector::aiGetVector(void)
{
	aiVector3D temp;
	temp.Set(x, y, z);
	return temp;
}
const D3DXVECTOR3 Vector::dxGetVector(void)
{
	return D3DXVECTOR3(x,y,z);
}

const D3DXVECTOR4 Vector::dxGetVector4(void)
{
	return D3DXVECTOR4(x,y,z,w);
}

Vector::~Vector(void)
{
}