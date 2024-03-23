#include "vec4.h"

#include "vec2.h"
#include "vec3.h"

#include <sstream>

vec4::vec4()
	: x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{
}

vec4::vec4(float f)
	: x(f), y(f), z(f), w(f)
{
}

vec4::vec4(float x, float y)
	: x(x), y(y), z(0.0f), w(0.0f)
{
}

vec4::vec4(float x, float y, float z)
	: x(x), y(y), z(z), w(0.0f)
{
}

vec4::vec4(float x, float y, float z, float w)
	: x(x), y(y), z(z), w(w)
{
}

vec4::vec4(const vec2& vec)
	: x(vec.x), y(vec.y), z(0.0f), w(0.0f)
{
}

vec4::vec4(const vec3& vec)
	: x(vec.x), y(vec.y), z(vec.z), w(0.0f)
{
}

vec4::vec4(const vec4& vec)
	: x(vec.x), y(vec.y), z(vec.z), w(vec.w)
{
}


vec4& vec4::Add(float f)
{
	x += f;
	y += f;
	z += f;
	w += f;
	return *this;
}

vec4& vec4::Add(float x, float y)
{
	this->x += x;
	this->y += y;
	return *this;
}

vec4& vec4::Add(float x, float y, float z)
{
	this->x += x;
	this->y += y;
	this->z += z;
	return *this;
}

vec4& vec4::Add(float x, float y, float z, float w)
{
	this->x += x;
	this->y += y;
	this->z += z;
	this->w += w;
	return *this;
}

vec4& vec4::Add(const vec2& vec)
{
	x += vec.x;
	y += vec.y;
	return *this;
}

vec4& vec4::Add(const vec3& vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;
	return *this;
}

vec4& vec4::Add(const vec4& vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;
	w += vec.w;
	return *this;
}


vec4& vec4::Subtract(float f)
{
	x -= f;
	y -= f;
	z -= f;
	w -= f;
	return *this;
}

vec4& vec4::Subtract(float x, float y)
{
	this->x -= x;
	this->y -= y;
	return *this;
}

vec4& vec4::Subtract(float x, float y, float z)
{
	this->x -= x;
	this->y -= y;
	this->z -= z;
	return *this;
}

vec4& vec4::Subtract(float x, float y, float z, float w)
{
	this->x -= x;
	this->y -= y;
	this->z -= z;
	this->w -= w;
	return *this;
}

vec4& vec4::Subtract(const vec2& vec)
{
	x -= vec.x;
	y -= vec.y;
	return *this;
}

vec4& vec4::Subtract(const vec3& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	return *this;
}

vec4& vec4::Subtract(const vec4& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	w -= vec.w;
	return *this;
}


vec4& vec4::Multiply(float f)
{
	x *= f;
	y *= f;
	z *= f;
	w *= f;
	return *this;
}

vec4& vec4::Multiply(float x, float y)
{
	this->x *= x;
	this->y *= y;
	return *this;
}

vec4& vec4::Multiply(float x, float y, float z)
{
	this->x *= x;
	this->y *= y;
	this->z *= z;
	return *this;
}

vec4& vec4::Multiply(float x, float y, float z, float w)
{
	this->x *= x;
	this->y *= y;
	this->z *= z;
	this->w *= w;
	return *this;
}

vec4& vec4::Multiply(const vec2& vec)
{
	x *= vec.x;
	y *= vec.y;
	return *this;
}

vec4& vec4::Multiply(const vec3& vec)
{
	x *= vec.x;
	y *= vec.y;
	z *= vec.z;
	return *this;
}

vec4& vec4::Multiply(const vec4& vec)
{
	x *= vec.x;
	y *= vec.y;
	z *= vec.z;
	w *= vec.w;
	return *this;
}


vec4& vec4::Divide(float f)
{
	f = 1.0f / f;
	x *= f;
	y *= f;
	z *= f;
	w *= f;
	return *this;
}

vec4& vec4::Divide(float x, float y)
{
	this->x /= x;
	this->y /= y;
	return *this;
}

vec4& vec4::Divide(float x, float y, float z)
{
	this->x /= x;
	this->y /= y;
	this->z /= z;
	return *this;
}

vec4& vec4::Divide(float x, float y, float z, float w)
{
	this->x /= x;
	this->y /= y;
	this->z /= z;
	this->w /= w;
	return *this;
}

vec4& vec4::Divide(const vec2& vec)
{
	x /= vec.x;
	y /= vec.y;
	return *this;
}

vec4& vec4::Divide(const vec3& vec)
{
	x /= vec.x;
	y /= vec.y;
	z /= vec.z;
	return *this;
}

vec4& vec4::Divide(const vec4& vec)
{
	x /= vec.x;
	y /= vec.y;
	z /= vec.z;
	w /= vec.w;
	return *this;
}


vec4 operator+(vec4 vec, float f)
{
	return vec.Add(f);
}

vec4 operator+(float f, vec4 vec)
{
	return vec.Add(f);
}

vec4 operator+(vec4 left, const vec4& right)
{
	return left.Add(right);
}


vec4 operator-(vec4 vec, float f)
{
	return vec.Subtract(f);
}

vec4 operator-(float f, vec4 vec)
{
	return vec.Subtract(f);
}

vec4 operator-(vec4 left, const vec4& right)
{
	return left.Subtract(right);
}


vec4 operator*(vec4 vec, float f)
{
	return vec.Multiply(f);
}

vec4 operator*(float f, vec4 vec)
{
	return vec.Multiply(f);
}

vec4 operator*(vec4 left, const vec4& right)
{
	return left.Multiply(right);
}


vec4 operator/(vec4 vec, float f)
{
	return vec.Divide(f);
}

vec4 operator/(float f, vec4 vec)
{
	return vec.Divide(f);
}

vec4 operator/(vec4 left, const vec4& right)
{
	return left.Divide(right);
}


bool vec4::operator<(const vec4& vec) const
{
	return (x < vec.x) && (y < vec.y) && (z < vec.z) && (w < vec.w);
}

bool vec4::operator<=(const vec4& vec) const
{
	return (x <= vec.x) && (y <= vec.y) && (z <= vec.z) && (w <= vec.w);
}

bool vec4::operator>(const vec4& vec) const
{
	return (x > vec.x) && (y > vec.y) && (z > vec.z) && (w > vec.w);
}

bool vec4::operator>=(const vec4& vec) const
{
	return (x >= vec.x) && (y >= vec.y) && (z >= vec.z) && (w >= vec.w);
}

bool vec4::operator==(const vec4& vec) const
{
	return (x == vec.x) && (y == vec.y) && (z == vec.z) && (w == vec.w);
}

bool vec4::operator!=(const vec4& vec) const
{
	return (x != vec.x) || (y != vec.y) || (z != vec.z) || (w != vec.w);
}


float vec4::Distance(const vec4& vec) const
{
	float dx = x - vec.x;
	float dy = x - vec.y;
	float dz = x - vec.z;
	float dw = x - vec.w;
	return dx * dx + dy * dy + dz * dz + dw * dw;
}

float vec4::Dot(const vec4& vec) const
{
	return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
}

float vec4::Magnitude() const
{
	return sqrt(x * x + y * y + z * z + w * w);
}

vec4& vec4::Normalize()
{
	return Divide(Magnitude());
}


std::string vec4::ToString() const
{
	std::stringstream ss;
	ss << "(" << x << ", " << y << ", " << z << ", " << w << ")";
	return ss.str();
}

std::ostream& operator<<(std::ostream& os, const vec4& vec)
{
	return os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ")";
}