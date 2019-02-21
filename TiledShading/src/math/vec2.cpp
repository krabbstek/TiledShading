#include "vec2.h"

#include "vec3.h"
#include "vec4.h"

#include <sstream>

vec2::vec2()
	: x(0.0f), y(0.0f)
{
}

vec2::vec2(float f)
	: x(f), y(f)
{
}

vec2::vec2(float x, float y)
	: x(x), y(y)
{
}

vec2::vec2(const vec2& vec)
	: x(vec.x), y(vec.y)
{
}

vec2::vec2(const vec3& vec)
	: x(vec.x), y(vec.y)
{
}

vec2::vec2(const vec4& vec)
	: x(vec.x), y(vec.y)
{
}


vec2& vec2::Add(float f)
{
	x += f;
	y += f;
	return *this;
}

vec2& vec2::Add(float x, float y)
{
	this->x += x;
	this->y += y;
	return *this;
}

vec2& vec2::Add(const vec2& vec)
{
	x += vec.x;
	y += vec.y;
	return *this;
}

vec2& vec2::Add(const vec3& vec)
{
	x += vec.x;
	y += vec.y;
	return *this;
}

vec2& vec2::Add(const vec4& vec)
{
	x += vec.x;
	y += vec.y;
	return *this;
}


vec2& vec2::Subtract(float f)
{
	x -= f;
	y -= f;
	return *this;
}

vec2& vec2::Subtract(float x, float y)
{
	this->x -= x;
	this->y -= y;
	return *this;
}

vec2& vec2::Subtract(const vec2& vec)
{
	x -= vec.x;
	y -= vec.y;
	return *this;
}

vec2& vec2::Subtract(const vec3& vec)
{
	x -= vec.x;
	y -= vec.y;
	return *this;
}

vec2& vec2::Subtract(const vec4& vec)
{
	x -= vec.x;
	y -= vec.y;
	return *this;
}


vec2& vec2::Multiply(float f)
{
	x *= f;
	y *= f;
	return *this;
}

vec2& vec2::Multiply(float x, float y)
{
	this->x *= x;
	this->y *= y;
	return *this;
}

vec2& vec2::Multiply(const vec2& vec)
{
	x *= vec.x;
	y *= vec.y;
	return *this;
}

vec2& vec2::Multiply(const vec3& vec)
{
	x *= vec.x;
	y *= vec.y;
	return *this;
}

vec2& vec2::Multiply(const vec4& vec)
{
	x *= vec.x;
	y *= vec.y;
	return *this;
}


vec2& vec2::Divide(float f)
{
	f = 1.0f / f;
	x *= f;
	y *= f;
	return *this;
}

vec2& vec2::Divide(float x, float y)
{
	this->x /= x;
	this->y /= y;
	return *this;
}

vec2& vec2::Divide(const vec2& vec)
{
	x /= vec.x;
	y /= vec.y;
	return *this;
}

vec2& vec2::Divide(const vec3& vec)
{
	x /= vec.x;
	y /= vec.y;
	return *this;
}

vec2& vec2::Divide(const vec4& vec)
{
	x /= vec.x;
	y /= vec.y;
	return *this;
}


vec2 operator+(vec2 vec, float f)
{
	return vec.Add(f);
}

vec2 operator+(float f, vec2 vec)
{
	return vec.Add(f);
}

vec2 operator+(vec2 left, const vec2& right)
{
	return left.Add(right);
}


vec2 operator-(vec2 vec, float f)
{
	return vec.Subtract(f);
}

vec2 operator-(float f, vec2 vec)
{
	return vec.Subtract(f);
}

vec2 operator-(vec2 left, const vec2& right)
{
	return left.Subtract(right);
}


vec2 operator*(vec2 vec, float f)
{
	return vec.Multiply(f);
}

vec2 operator*(float f, vec2 vec)
{
	return vec.Multiply(f);
}

vec2 operator*(vec2 left, const vec2& right)
{
	return left.Multiply(right);
}


vec2 operator/(vec2 vec, float f)
{
	return vec.Divide(f);
}

vec2 operator/(float f, vec2 vec)
{
	return vec.Divide(f);
}

vec2 operator/(vec2 left, const vec2& right)
{
	return left.Divide(right);
}


bool vec2::operator<(const vec2& vec) const
{
	return (x < vec.x) && (y < vec.y);
}

bool vec2::operator<=(const vec2& vec) const
{
	return (x <= vec.x) && (y <= vec.y);
}

bool vec2::operator>(const vec2& vec) const
{
	return (x > vec.x) && (y > vec.y);
}

bool vec2::operator>=(const vec2& vec) const
{
	return (x >= vec.x) && (y >= vec.y);
}

bool vec2::operator==(const vec2& vec) const
{
	return (x == vec.x) && (y == vec.y);
}

bool vec2::operator!=(const vec2& vec) const
{
	return (x != vec.x) || (y != vec.y);
}


float vec2::Distance(const vec2& vec) const
{
	float dx = x - vec.x;
	float dy = y - vec.y;
	return sqrt(dx * dx + dy * dy);
}

float vec2::Dot(const vec2& vec) const
{
	return x * vec.x + y * vec.y;
}

float vec2::Magnitude() const
{
	return sqrt(x * x + y * y);
}

vec2& vec2::Normalize()
{
	return Divide(Magnitude());
}


std::string vec2::ToString() const
{
	std::stringstream ss;
	ss << "(" << x << ", " << y << ")";
	return ss.str();
}

std::ostream& operator<<(std::ostream& os, const vec2& vec)
{
	return os << "(" << vec.x << ", " << vec.y << ")";
}