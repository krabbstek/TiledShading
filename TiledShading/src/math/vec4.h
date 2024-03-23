#pragma once

#include "vec2.h"
#include "vec3.h"

struct vec4
{
public:
	union
	{
		struct
		{
			float x, y, z, w;
		};
		struct
		{
			float r, g, b, a;
		};
		vec2 xy;
		vec3 xyz;
	};

public:
	vec4();
	vec4(float f);
	vec4(float x, float y);
	vec4(float x, float y, float z);
	vec4(float x, float y, float z, float w);
	vec4(const vec2& vec);
	vec4(const vec3& vec);
	vec4(const vec4& vec);

	vec4& Add(float f);
	vec4& Add(float x, float y);
	vec4& Add(float x, float y, float z);
	vec4& Add(float x, float y, float z, float w);
	vec4& Add(const vec2& vec);
	vec4& Add(const vec3& vec);
	vec4& Add(const vec4& vec);

	vec4& Subtract(float f);
	vec4& Subtract(float x, float y);
	vec4& Subtract(float x, float y, float z);
	vec4& Subtract(float x, float y, float z, float w);
	vec4& Subtract(const vec2& vec);
	vec4& Subtract(const vec3& vec);
	vec4& Subtract(const vec4& vec);

	vec4& Multiply(float f);
	vec4& Multiply(float x, float y);
	vec4& Multiply(float x, float y, float z);
	vec4& Multiply(float x, float y, float z, float w);
	vec4& Multiply(const vec2& vec);
	vec4& Multiply(const vec3& vec);
	vec4& Multiply(const vec4& vec);

	vec4& Divide(float f);
	vec4& Divide(float x, float y);
	vec4& Divide(float x, float y, float z);
	vec4& Divide(float x, float y, float z, float w);
	vec4& Divide(const vec2& vec);
	vec4& Divide(const vec3& vec);
	vec4& Divide(const vec4& vec);

	inline vec4& operator+=(float f) { return Add(f); }
	inline vec4& operator+=(const vec2& vec) { return Add(vec); }
	inline vec4& operator+=(const vec3& vec) { return Add(vec); }
	inline vec4& operator+=(const vec4& vec) { return Add(vec); }

	inline vec4& operator-=(float f) { return Subtract(f); }
	inline vec4& operator-=(const vec2& vec) { return Subtract(vec); }
	inline vec4& operator-=(const vec3& vec) { return Subtract(vec); }
	inline vec4& operator-=(const vec4& vec) { return Subtract(vec); }

	inline vec4& operator*=(float f) { return Multiply(f); }
	inline vec4& operator*=(const vec2& vec) { return Multiply(vec); }
	inline vec4& operator*=(const vec3& vec) { return Multiply(vec); }
	inline vec4& operator*=(const vec4& vec) { return Multiply(vec); }

	inline vec4& operator/=(float f) { return Divide(f); }
	inline vec4& operator/=(const vec2& vec) { return Divide(vec); }
	inline vec4& operator/=(const vec3& vec) { return Divide(vec); }
	inline vec4& operator/=(const vec4& vec) { return Divide(vec); }

	friend vec4 operator+(vec4 vec, float f);
	friend vec4 operator+(float f, vec4 vec);
	friend vec4 operator+(vec4 left, const vec4& right);

	friend vec4 operator-(vec4 vec, float f);
	friend vec4 operator-(float f, vec4 vec);
	friend vec4 operator-(vec4 left, const vec4& right);

	friend vec4 operator*(vec4 vec, float f);
	friend vec4 operator*(float f, vec4 vec);
	friend vec4 operator*(vec4 left, const vec4& right);

	friend vec4 operator/(vec4 vec, float f);
	friend vec4 operator/(float f, vec4 vec);
	friend vec4 operator/(vec4 left, const vec4& right);

	inline vec4 operator-() const { return vec4(-x, -y, -z, -w); }

	bool operator<(const vec4& vec) const;
	bool operator<=(const vec4& vec) const;
	bool operator>(const vec4& vec) const;
	bool operator>=(const vec4& vec) const;
	bool operator==(const vec4& vec) const;
	bool operator!=(const vec4& vec) const;

	float Distance(const vec4& vec) const;
	float Dot(const vec4& vec) const;
	inline static float Dot(const vec4& left, const vec4& right) { return left.Dot(right); }
	float Magnitude() const;
	vec4& Normalize();
	inline static vec4 Normalize(vec4 vec) { return vec.Normalize(); }

	std::string ToString() const;
	friend std::ostream& operator<<(std::ostream& os, const vec4& vec);
};
