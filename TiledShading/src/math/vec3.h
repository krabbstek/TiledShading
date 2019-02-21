#pragma once

#include "vec2.h"

struct vec4;

struct vec3
{
public:
	union
	{
		struct
		{
			float x, y, z;
		};
		struct
		{
			float u, v, w;
		};
		struct
		{
			float r, g, b;
		};
		vec2 xy;
	};

public:
	vec3();
	vec3(float f);
	vec3(float x, float y);
	vec3(float x, float y, float z);
	vec3(const vec2& vec);
	vec3(const vec3& vec);
	vec3(const vec4& vec);

	inline static vec3 X() { return vec3(1.0f, 0.0f, 0.0f); }
	inline static vec3 Y() { return vec3(0.0f, 1.0f, 0.0f); }
	inline static vec3 Z() { return vec3(0.0f, 0.0f, 1.0f); }

	vec3& Add(float f);
	vec3& Add(float x, float y);
	vec3& Add(float x, float y, float z);
	vec3& Add(const vec2& vec);
	vec3& Add(const vec3& vec);
	vec3& Add(const vec4& vec);

	vec3& Subtract(float f);
	vec3& Subtract(float x, float y);
	vec3& Subtract(float x, float y, float z);
	vec3& Subtract(const vec2& vec);
	vec3& Subtract(const vec3& vec);
	vec3& Subtract(const vec4& vec);

	vec3& Multiply(float f);
	vec3& Multiply(float x, float y);
	vec3& Multiply(float x, float y, float z);
	vec3& Multiply(const vec2& vec);
	vec3& Multiply(const vec3& vec);
	vec3& Multiply(const vec4& vec);

	vec3& Divide(float f);
	vec3& Divide(float x, float y);
	vec3& Divide(float x, float y, float z);
	vec3& Divide(const vec2& vec);
	vec3& Divide(const vec3& vec);
	vec3& Divide(const vec4& vec);

	inline vec3& operator+=(float f) { return Add(f); }
	inline vec3& operator+=(const vec2& vec) { return Add(vec); }
	inline vec3& operator+=(const vec3& vec) { return Add(vec); }
	inline vec3& operator+=(const vec4& vec) { return Add(vec); }

	inline vec3& operator-=(float f) { return Subtract(f); }
	inline vec3& operator-=(const vec2& vec) { return Subtract(vec); }
	inline vec3& operator-=(const vec3& vec) { return Subtract(vec); }
	inline vec3& operator-=(const vec4& vec) { return Subtract(vec); }

	inline vec3& operator*=(float f) { return Multiply(f); }
	inline vec3& operator*=(const vec2& vec) { return Multiply(vec); }
	inline vec3& operator*=(const vec3& vec) { return Multiply(vec); }
	inline vec3& operator*=(const vec4& vec) { return Multiply(vec); }

	inline vec3& operator/=(float f) { return Divide(f); }
	inline vec3& operator/=(const vec2& vec) { return Divide(vec); }
	inline vec3& operator/=(const vec3& vec) { return Divide(vec); }
	inline vec3& operator/=(const vec4& vec) { return Divide(vec); }

	friend vec3 operator+(vec3 vec, float f);
	friend vec3 operator+(float f, vec3 vec);
	friend vec3 operator+(vec3 left, const vec3& right);

	friend vec3 operator-(vec3 vec, float f);
	friend vec3 operator-(float f, vec3 vec);
	friend vec3 operator-(vec3 left, const vec3& right);

	friend vec3 operator*(vec3 vec, float f);
	friend vec3 operator*(float f, vec3 vec);
	friend vec3 operator*(vec3 left, const vec3& right);

	friend vec3 operator/(vec3 vec, float f);
	friend vec3 operator/(float f, vec3 vec);
	friend vec3 operator/(vec3 left, const vec3& right);

	inline vec3 operator-() const { return vec3(-x, -y, -z); }

	bool operator<(const vec3& vec) const;
	bool operator<=(const vec3& vec) const;
	bool operator>(const vec3& vec) const;
	bool operator>=(const vec3& vec) const;
	bool operator==(const vec3& vec) const;
	bool operator!=(const vec3& vec) const;

	float Distance(const vec3& vec) const;
	float Dot(const vec3& vec) const;
	inline static float Dot(const vec3& left, const vec3& right) { return left.Dot(right); }
	vec3 Cross(const vec3& right) const;
	inline static vec3 Cross(const vec3& left, const vec3& right) { return left.Cross(right); }
	float Magnitude() const;
	vec3& Normalize();
	inline static vec3 Normalize(vec3 vec) { return vec.Normalize(); }

	std::string ToString() const;
	friend std::ostream& operator<<(std::ostream& os, const vec3& vec);
};