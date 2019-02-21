#pragma once

#include <string>
#include <ostream>

struct vec3;
struct vec4;

struct vec2
{
public:
	union
	{
		struct
		{
			float x, y;
		};
		struct
		{
			float u, v;
		};
	};

public:
	vec2();
	vec2(float f);
	vec2(float x, float y);
	vec2(const vec2& vec);
	vec2(const vec3& vec);
	vec2(const vec4& vec);

	vec2& Add(float f);
	vec2& Add(float x, float y);
	vec2& Add(const vec2& vec);
	vec2& Add(const vec3& vec);
	vec2& Add(const vec4& vec);

	vec2& Subtract(float f);
	vec2& Subtract(float x, float y);
	vec2& Subtract(const vec2& vec);
	vec2& Subtract(const vec3& vec);
	vec2& Subtract(const vec4& vec);

	vec2& Multiply(float f);
	vec2& Multiply(float x, float y);
	vec2& Multiply(const vec2& vec);
	vec2& Multiply(const vec3& vec);
	vec2& Multiply(const vec4& vec);

	vec2& Divide(float f);
	vec2& Divide(float x, float y);
	vec2& Divide(const vec2& vec);
	vec2& Divide(const vec3& vec);
	vec2& Divide(const vec4& vec);

	inline vec2& operator+=(float f) { return Add(f); }
	inline vec2& operator+=(const vec2& vec) { return Add(vec); }
	inline vec2& operator+=(const vec3& vec) { return Add(vec); }
	inline vec2& operator+=(const vec4& vec) { return Add(vec); }

	inline vec2& operator-=(float f) { return Subtract(f); }
	inline vec2& operator-=(const vec2& vec) { return Subtract(vec); }
	inline vec2& operator-=(const vec3& vec) { return Subtract(vec); }
	inline vec2& operator-=(const vec4& vec) { return Subtract(vec); }

	inline vec2& operator*=(float f) { return Multiply(f); }
	inline vec2& operator*=(const vec2& vec) { return Multiply(vec); }
	inline vec2& operator*=(const vec3& vec) { return Multiply(vec); }
	inline vec2& operator*=(const vec4& vec) { return Multiply(vec); }

	inline vec2& operator/=(float f) { return Divide(f); }
	inline vec2& operator/=(const vec2& vec) { return Divide(vec); }
	inline vec2& operator/=(const vec3& vec) { return Divide(vec); }
	inline vec2& operator/=(const vec4& vec) { return Divide(vec); }

	friend vec2 operator+(vec2 vec, float f);
	friend vec2 operator+(float f, vec2 vec);
	friend vec2 operator+(vec2 left, const vec2& right);

	friend vec2 operator-(vec2 vec, float f);
	friend vec2 operator-(float f, vec2 vec);
	friend vec2 operator-(vec2 left, const vec2& right);

	friend vec2 operator*(vec2 vec, float f);
	friend vec2 operator*(float f, vec2 vec);
	friend vec2 operator*(vec2 left, const vec2& right);

	friend vec2 operator/(vec2 vec, float f);
	friend vec2 operator/(float f, vec2 vec);
	friend vec2 operator/(vec2 left, const vec2& right);

	inline vec2 operator-() const { return vec2(-x, -y); }

	bool operator<(const vec2& vec) const;
	bool operator<=(const vec2& vec) const;
	bool operator>(const vec2& vec) const;
	bool operator>=(const vec2& vec) const;
	bool operator==(const vec2& vec) const;
	bool operator!=(const vec2& vec) const;

	float Distance(const vec2& vec) const;
	float Dot(const vec2& vec) const;
	inline static float Dot(const vec2& left, const vec2& right) { return left.Dot(right); }
	float Magnitude() const;
	vec2& Normalize();
	inline static vec2 Normalize(vec2 vec) { return vec.Normalize(); }

	std::string ToString() const;
	friend std::ostream& operator<<(std::ostream& os, const vec2& vec);
};
