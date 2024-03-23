#include "vec3.h"

#include "vec2.h"
#include "vec4.h"

#include <sstream>

vec3::vec3()
	: x(0.0f), y(0.0f), z(0.0f)
{
}

vec3::vec3(float f)
	: x(f), y(f), z(f)
{
}

vec3::vec3(float x, float y)
	: x(x), y(y), z(0.0f)
{
}

vec3::vec3(float x, float y, float z)
	: x(x), y(y), z(z)
{
}

vec3::vec3(const vec2& vec)
	: x(vec.x), y(vec.y), z(0.0f)
{
}

vec3::vec3(const vec3& vec)
	: x(vec.x), y(vec.y), z(vec.z)
{
}

vec3::vec3(const vec4& vec)
	: x(vec.x), y(vec.y), z(vec.z)
{
}


vec3& vec3::Add(float f)
{
	x += f;
	y += f;
	z += f;
	return *this;
}

vec3& vec3::Add(float x, float y)
{
	this->x += x;
	this->y += y;
	return *this;
}

vec3& vec3::Add(float x, float y, float z)
{
	this->x += x;
	this->y += y;
	this->z += z;
	return *this;
}

vec3& vec3::Add(const vec2& vec)
{
	x += vec.x;
	y += vec.y;
	return *this;
}

vec3& vec3::Add(const vec3& vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;
	return *this;
}

vec3& vec3::Add(const vec4& vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;
	return *this;
}


vec3& vec3::Subtract(float f)
{
	x -= f;
	y -= f;
	z -= f;
	return *this;
}

vec3& vec3::Subtract(float x, float y)
{
	this->x -= x;
	this->y -= y;
	return *this;
}

vec3& vec3::Subtract(float x, float y, float z)
{
	this->x -= x;
	this->y -= y;
	this->z -= z;
	return *this;
}

vec3& vec3::Subtract(const vec2& vec)
{
	x -= vec.x;
	y -= vec.y;
	return *this;
}

vec3& vec3::Subtract(const vec3& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	return *this;
}

vec3& vec3::Subtract(const vec4& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	return *this;
}


vec3& vec3::Multiply(float f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

vec3& vec3::Multiply(float x, float y)
{
	this->x *= x;
	this->y *= y;
	return *this;
}

vec3& vec3::Multiply(float x, float y, float z)
{
	this->x *= x;
	this->y *= y;
	this->z *= z;
	return *this;
}

vec3& vec3::Multiply(const vec2& vec)
{
	x *= vec.x;
	y *= vec.y;
	return *this;
}

vec3& vec3::Multiply(const vec3& vec)
{
	x *= vec.x;
	y *= vec.y;
	z *= vec.z;
	return *this;
}

vec3& vec3::Multiply(const vec4& vec)
{
	x *= vec.x;
	y *= vec.y;
	z *= vec.z;
	return *this;
}


vec3& vec3::Divide(float f)
{
	f = 1.0f / f;
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

vec3& vec3::Divide(float x, float y)
{
	this->x /= x;
	this->y /= y;
	return *this;
}

vec3& vec3::Divide(float x, float y, float z)
{
	this->x /= x;
	this->y /= y;
	this->z /= z;
	return *this;
}

vec3& vec3::Divide(const vec2& vec)
{
	x /= vec.x;
	y /= vec.y;
	return *this;
}

vec3& vec3::Divide(const vec3& vec)
{
	x /= vec.x;
	y /= vec.y;
	z /= vec.z;
	return *this;
}

vec3& vec3::Divide(const vec4& vec)
{
	x /= vec.x;
	y /= vec.y;
	z /= vec.z;
	return *this;
}


vec3 operator+(vec3 vec, float f)
{
	return vec.Add(f);
}

vec3 operator+(float f, vec3 vec)
{
	return vec.Add(f);
}

vec3 operator+(vec3 left, const vec3& right)
{
	return left.Add(right);
}


vec3 operator-(vec3 vec, float f)
{
	return vec.Subtract(f);
}

vec3 operator-(float f, vec3 vec)
{
	return vec.Subtract(f);
}

vec3 operator-(vec3 left, const vec3& right)
{
	return left.Subtract(right);
}


vec3 operator*(vec3 vec, float f)
{
	return vec.Multiply(f);
}

vec3 operator*(float f, vec3 vec)
{
	return vec.Multiply(f);
}

vec3 operator*(vec3 left, const vec3& right)
{
	return left.Multiply(right);
}


vec3 operator/(vec3 vec, float f)
{
	return vec.Divide(f);
}

vec3 operator/(float f, vec3 vec)
{
	return vec.Divide(f);
}

vec3 operator/(vec3 left, const vec3& right)
{
	return left.Divide(right);
}


bool vec3::operator<(const vec3& vec) const
{
	return (x < vec.x) && (y < vec.y) && (z < vec.z);
}

bool vec3::operator<=(const vec3& vec) const
{
	return (x <= vec.x) && (y <= vec.y) && (z <= vec.z);
}

bool vec3::operator>(const vec3& vec) const
{
	return (x > vec.x) && (y > vec.y) && (z > vec.z);
}

bool vec3::operator>=(const vec3& vec) const
{
	return (x >= vec.x) && (y >= vec.y) && (z >= vec.z);
}

bool vec3::operator==(const vec3& vec) const
{
	return (x == vec.x) && (y == vec.y) && (z == vec.z);
}

bool vec3::operator!=(const vec3& vec) const
{
	return (x != vec.x) || (y != vec.y) || (z != vec.z);
}


float vec3::Distance(const vec3& vec) const
{
	float dx = x - vec.x;
	float dy = y - vec.y;
	float dz = z - vec.z;
	return sqrt(dx * dx + dy * dy + dz * dz);
}

float vec3::Dot(const vec3& vec) const
{
	return x * vec.x + y * vec.y + z * vec.z;
}

vec3 vec3::Cross(const vec3& right) const
{
	float nx = y * right.z - z * right.y;
	float ny = z * right.x - x * right.z;
	float nz = x * right.y - y * right.x;
	return vec3(nx, ny, nz);
}

float vec3::Magnitude() const
{
	return sqrt(x * x + y * y + z * z);
}

vec3& vec3::Normalize()
{
	return Divide(Magnitude());
}


std::string vec3::ToString() const
{
	std::stringstream ss;
	ss << "(" << x << ", " << y << ", " << z << ")";
	return ss.str();
}

std::ostream& operator<<(std::ostream& os, const vec3& vec)
{
	return os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
}