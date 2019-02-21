#include "mat4.h"

#define INDEX(ROW, COL) (ROW + (COL << 2))

mat4::mat4()
{
	memset(elements, 0, sizeof(elements));
}

mat4::mat4(float diagonal)
{
	memset(elements, 0, sizeof(elements));
	elements[INDEX(0, 0)] = diagonal;
	elements[INDEX(1, 1)] = diagonal;
	elements[INDEX(2, 2)] = diagonal;
	elements[INDEX(3, 3)] = diagonal;
}

mat4::mat4(float dx, float dy, float dz)
{
	memset(elements, 0, sizeof(elements));
	elements[INDEX(0, 0)] = dx;
	elements[INDEX(1, 1)] = dy;
	elements[INDEX(2, 2)] = dz;
	elements[INDEX(3, 3)] = 1.0f;
}

mat4::mat4(float dx, float dy, float dz, float dw)
{
	memset(elements, 0, sizeof(elements));
	elements[INDEX(0, 0)] = dx;
	elements[INDEX(1, 1)] = dy;
	elements[INDEX(2, 2)] = dz;
	elements[INDEX(3, 3)] = dw;
}

mat4::mat4(const vec3& diagonal)
{
	memset(elements, 0, sizeof(elements));
	elements[INDEX(0, 0)] = diagonal.x;
	elements[INDEX(1, 1)] = diagonal.y;
	elements[INDEX(2, 2)] = diagonal.z;
	elements[INDEX(3, 3)] = 1.0f;
}

mat4::mat4(const vec4& diagonal)
{
	memset(elements, 0, sizeof(elements));
	elements[INDEX(0, 0)] = diagonal.x;
	elements[INDEX(1, 1)] = diagonal.y;
	elements[INDEX(2, 2)] = diagonal.z;
	elements[INDEX(3, 3)] = diagonal.w;
}

mat4::mat4(const vec3& col0, const vec3& col1, const vec3& col2)
{
	elements[INDEX(0, 0)] = col0.x;
	elements[INDEX(1, 0)] = col0.y;
	elements[INDEX(2, 0)] = col0.z;
	elements[INDEX(3, 0)] = 0.0f;

	elements[INDEX(0, 1)] = col1.x;
	elements[INDEX(1, 1)] = col1.y;
	elements[INDEX(2, 1)] = col1.z;
	elements[INDEX(3, 1)] = 0.0f;

	elements[INDEX(0, 2)] = col2.x;
	elements[INDEX(1, 2)] = col2.y;
	elements[INDEX(2, 2)] = col2.z;
	elements[INDEX(3, 2)] = 0.0f;

	elements[INDEX(0, 2)] = 0.0f;
	elements[INDEX(1, 2)] = 0.0f;
	elements[INDEX(2, 2)] = 0.0f;
	elements[INDEX(3, 2)] = 0.0f;
}

mat4::mat4(const vec3& col0, const vec3& col1, const vec3& col2, const vec3& col3)
{
	elements[INDEX(0, 0)] = col0.x;
	elements[INDEX(1, 0)] = col0.y;
	elements[INDEX(2, 0)] = col0.z;
	elements[INDEX(3, 0)] = 0.0f;

	elements[INDEX(0, 1)] = col1.x;
	elements[INDEX(1, 1)] = col1.y;
	elements[INDEX(2, 1)] = col1.z;
	elements[INDEX(3, 1)] = 0.0f;

	elements[INDEX(0, 2)] = col2.x;
	elements[INDEX(1, 2)] = col2.y;
	elements[INDEX(2, 2)] = col2.z;
	elements[INDEX(3, 2)] = 0.0f;

	elements[INDEX(0, 2)] = col3.x;
	elements[INDEX(1, 2)] = col3.y;
	elements[INDEX(2, 2)] = col3.z;
	elements[INDEX(3, 2)] = 0.0f;
}

mat4::mat4(const vec4& col0, const vec4& col1, const vec4& col2, const vec4& col3)
{
	elements[INDEX(0, 0)] = col0.x;
	elements[INDEX(1, 0)] = col0.y;
	elements[INDEX(2, 0)] = col0.z;
	elements[INDEX(3, 0)] = col0.w;

	elements[INDEX(0, 1)] = col1.x;
	elements[INDEX(1, 1)] = col1.y;
	elements[INDEX(2, 1)] = col1.z;
	elements[INDEX(3, 1)] = col1.w;

	elements[INDEX(0, 2)] = col2.x;
	elements[INDEX(1, 2)] = col2.y;
	elements[INDEX(2, 2)] = col2.z;
	elements[INDEX(3, 2)] = col2.w;

	elements[INDEX(0, 2)] = col3.x;
	elements[INDEX(1, 2)] = col3.y;
	elements[INDEX(2, 2)] = col3.z;
	elements[INDEX(3, 2)] = col3.w;
}

mat4::mat4(const float* elements)
{
	memcpy(this->elements, elements, sizeof(this->elements));
}


mat4 mat4::Translate(float t)
{
	mat4 result;

	result.elements[INDEX(0, 0)] = 1.0f;
	result.elements[INDEX(1, 1)] = 1.0f;
	result.elements[INDEX(2, 2)] = 1.0f;
	result.elements[INDEX(3, 3)] = 1.0f;

	result.elements[INDEX(0, 3)] = t;
	result.elements[INDEX(1, 3)] = t;
	result.elements[INDEX(2, 3)] = t;

	return result;
}

mat4 mat4::Translate(float tx, float ty, float tz)
{
	mat4 result;

	result.elements[INDEX(0, 0)] = 1.0f;
	result.elements[INDEX(1, 1)] = 1.0f;
	result.elements[INDEX(2, 2)] = 1.0f;
	result.elements[INDEX(3, 3)] = 1.0f;

	result.elements[INDEX(0, 3)] = tx;
	result.elements[INDEX(1, 3)] = ty;
	result.elements[INDEX(2, 3)] = tz;

	return result;
}

mat4 mat4::Translate(const vec3& t)
{
	mat4 result;

	result.elements[INDEX(0, 0)] = 1.0f;
	result.elements[INDEX(1, 1)] = 1.0f;
	result.elements[INDEX(2, 2)] = 1.0f;
	result.elements[INDEX(3, 3)] = 1.0f;

	result.elements[INDEX(0, 3)] = t.x;
	result.elements[INDEX(1, 3)] = t.y;
	result.elements[INDEX(2, 3)] = t.z;

	return result;
}

mat4 mat4::Translate(const vec4& t)
{
	mat4 result;

	result.elements[INDEX(0, 0)] = 1.0f;
	result.elements[INDEX(1, 1)] = 1.0f;
	result.elements[INDEX(2, 2)] = 1.0f;
	result.elements[INDEX(3, 3)] = 1.0f;

	result.elements[INDEX(0, 3)] = t.x;
	result.elements[INDEX(1, 3)] = t.y;
	result.elements[INDEX(2, 3)] = t.z;

	return result;
}


mat4 mat4::RotateX(float radians)
{
	mat4 result;

	float s = sin(radians);
	float c = cos(radians);

	result.elements[INDEX(0, 0)] = 1.0f;

	result.elements[INDEX(1, 1)] = c;
	result.elements[INDEX(1, 2)] = -s;
	result.elements[INDEX(2, 1)] = s;
	result.elements[INDEX(2, 2)] = c;

	result.elements[INDEX(3, 3)] = 1.0f;

	return result;
}

mat4 mat4::RotateY(float radians)
{
	mat4 result;

	float s = sin(radians);
	float c = cos(radians);

	result.elements[INDEX(0, 0)] = c;
	result.elements[INDEX(0, 2)] = s;
	result.elements[INDEX(2, 0)] = -s;
	result.elements[INDEX(2, 2)] = c;

	result.elements[INDEX(1, 1)] = 1.0f;

	result.elements[INDEX(3, 3)] = 1.0f;

	return result;
}

mat4 mat4::RotateZ(float radians)
{
	mat4 result;

	float s = sin(radians);
	float c = cos(radians);

	result.elements[INDEX(0, 0)] = c;
	result.elements[INDEX(0, 1)] = -s;
	result.elements[INDEX(1, 0)] = s;
	result.elements[INDEX(1, 1)] = c;

	result.elements[INDEX(2, 2)] = 1.0f;

	result.elements[INDEX(3, 3)] = 1.0f;

	return result;
}

mat4 mat4::Rotate(float radians, float ax, float ay, float az)
{
	mat4 result;

	float c = cos(radians);
	float nc = 1.0f - c;
	float s = sin(radians);

	float ax2 = ax * ax;
	float axy = ax * ay;
	float axz = ax * az;
	float ay2 = ay * ay;
	float ayz = ay * az;
	float az2 = az * az;

	float axy_nc = axy * nc;
	float axz_nc = axz * nc;
	float ayz_nc = ayz * nc;

	float ax_s = ax * s;
	float ay_s = ay * s;
	float az_s = az * s;

	result.elements[INDEX(0, 0)] = c + ax2 * nc;
	result.elements[INDEX(0, 1)] = axy_nc - az_s;
	result.elements[INDEX(0, 2)] = axz_nc + ay_s;

	result.elements[INDEX(1, 0)] = axy_nc + az_s;
	result.elements[INDEX(1, 1)] = c + ay2 * nc;
	result.elements[INDEX(1, 2)] = ayz_nc - ax_s;

	result.elements[INDEX(2, 0)] = axz_nc - ay_s;
	result.elements[INDEX(2, 1)] = ayz_nc + ax_s;
	result.elements[INDEX(2, 2)] = c + az2 * nc;

	result.elements[INDEX(0, 3)] = 0.0f;
	result.elements[INDEX(1, 3)] = 0.0f;
	result.elements[INDEX(2, 3)] = 0.0f;

	result.elements[INDEX(3, 0)] = 0.0f;
	result.elements[INDEX(3, 1)] = 0.0f;
	result.elements[INDEX(3, 2)] = 0.0f;

	result.elements[INDEX(3, 3)] = 1.0f;

	return result;
}

mat4 mat4::Rotate(float radians, const vec3& axis)
{
	mat4 result;

	float c = cos(radians);
	float nc = 1.0f - c;
	float s = sin(radians);

	float ax2 = axis.x * axis.x;
	float axy = axis.x * axis.y;
	float axz = axis.x * axis.z;
	float ay2 = axis.y * axis.y;
	float ayz = axis.y * axis.z;
	float az2 = axis.z * axis.z;

	float axy_nc = axy * nc;
	float axz_nc = axz * nc;
	float ayz_nc = ayz * nc;

	float ax_s = axis.x * s;
	float ay_s = axis.y * s;
	float az_s = axis.z * s;

	result.elements[INDEX(0, 0)] = c + ax2 * nc;
	result.elements[INDEX(0, 1)] = axy_nc - az_s;
	result.elements[INDEX(0, 2)] = axz_nc + ay_s;
		
	result.elements[INDEX(1, 0)] = axy_nc + az_s;
	result.elements[INDEX(1, 1)] = c + ay2 * nc;
	result.elements[INDEX(1, 2)] = ayz_nc - ax_s;
		
	result.elements[INDEX(2, 0)] = axz_nc - ay_s;
	result.elements[INDEX(2, 1)] = ayz_nc + ax_s;
	result.elements[INDEX(2, 2)] = c + az2 * nc;
		
	result.elements[INDEX(0, 3)] = 0.0f;
	result.elements[INDEX(1, 3)] = 0.0f;
	result.elements[INDEX(2, 3)] = 0.0f;
		
	result.elements[INDEX(3, 0)] = 0.0f;
	result.elements[INDEX(3, 1)] = 0.0f;
	result.elements[INDEX(3, 2)] = 0.0f;
		
	result.elements[INDEX(3, 3)] = 1.0f;

	return result;
}

mat4 mat4::Rotate(float radians, const vec4& axis)
{
	mat4 result;

	float c = cos(radians);
	float nc = 1.0f - c;
	float s = sin(radians);

	float ax2 = axis.x * axis.x;
	float axy = axis.x * axis.y;
	float axz = axis.x * axis.z;
	float ay2 = axis.y * axis.y;
	float ayz = axis.y * axis.z;
	float az2 = axis.z * axis.z;

	float axy_nc = axy * nc;
	float axz_nc = axz * nc;
	float ayz_nc = ayz * nc;

	float ax_s = axis.x * s;
	float ay_s = axis.y * s;
	float az_s = axis.z * s;

	result.elements[INDEX(0, 0)] = c + ax2 * nc;
	result.elements[INDEX(0, 1)] = axy_nc - az_s;
	result.elements[INDEX(0, 2)] = axz_nc + ay_s;
		
	result.elements[INDEX(1, 0)] = axy_nc + az_s;
	result.elements[INDEX(1, 1)] = c + ay2 * nc;
	result.elements[INDEX(1, 2)] = ayz_nc - ax_s;
		
	result.elements[INDEX(2, 0)] = axz_nc - ay_s;
	result.elements[INDEX(2, 1)] = ayz_nc + ax_s;
	result.elements[INDEX(2, 2)] = c + az2 * nc;
		
	result.elements[INDEX(0, 3)] = 0.0f;
	result.elements[INDEX(1, 3)] = 0.0f;
	result.elements[INDEX(2, 3)] = 0.0f;
		
	result.elements[INDEX(3, 0)] = 0.0f;
	result.elements[INDEX(3, 1)] = 0.0f;
	result.elements[INDEX(3, 2)] = 0.0f;
		
	result.elements[INDEX(3, 3)] = 1.0f;

	return result;
}

mat4 mat4::Orthographic(float left, float right, float bottom, float top, float near, float far)
{
	mat4 result;

	float _rl = 1.0f / (right - left);
	float _tb = 1.0f / (top - bottom);
	float _nf = 1.0f / (near - far);

	result.elements[INDEX(0, 0)] = 2.0f * _rl;
	result.elements[INDEX(1, 1)] = 2.0f * _tb;
	result.elements[INDEX(2, 2)] = 2.0f * _nf;

	result.elements[INDEX(0, 3)] = -(right + left) * _rl;
	result.elements[INDEX(1, 3)] = -(top + bottom) * _tb;
	result.elements[INDEX(2, 3)] = (far + near) * _nf;

	result.elements[INDEX(3, 3)] = 1.0f;

	return result;
}

mat4 mat4::Perspective(float fov, float aspectRatio, float near, float far)
{
	mat4 result;

	float t_inv = 1.0f / tan(0.5f * fov);
	float nearfar_inv = 1.0f / (near - far);

	result.elements[INDEX(0, 0)] = t_inv / aspectRatio;
	result.elements[INDEX(1, 1)] = t_inv;

	result.elements[INDEX(2, 2)] = (far + near) * nearfar_inv;
	result.elements[INDEX(2, 3)] = 2.0f * far * near * nearfar_inv;

	result.elements[INDEX(3, 2)] = -1.0f;

	return result;
}

mat4 mat4::LookAt(const vec3& cameraPosition, const vec3& objectPosition, const vec3& up)
{
	vec3 zAxis = vec3::Normalize(objectPosition - cameraPosition);
	vec3 xAxis = vec3::Normalize(vec3::Cross(up, zAxis));
	vec3 yAxis = vec3::Cross(zAxis, xAxis);
	vec3 t(vec3::Dot(xAxis, cameraPosition), vec3::Dot(yAxis, cameraPosition), vec3::Dot(zAxis, cameraPosition));

	mat4 result;

	result.elements[INDEX(0, 0)] = xAxis.x;
	result.elements[INDEX(1, 0)] = yAxis.x;
	result.elements[INDEX(2, 0)] = zAxis.x;
		
	result.elements[INDEX(0, 1)] = xAxis.y;
	result.elements[INDEX(1, 1)] = yAxis.y;
	result.elements[INDEX(2, 1)] = zAxis.y;
		
	result.elements[INDEX(0, 2)] = xAxis.z;
	result.elements[INDEX(1, 2)] = yAxis.z;
	result.elements[INDEX(2, 2)] = zAxis.z;
		
	result.elements[INDEX(0, 3)] = t.x;
	result.elements[INDEX(1, 3)] = t.y;
	result.elements[INDEX(2, 3)] = t.z;
		
	result.elements[INDEX(3, 0)] = 0.0f;
	result.elements[INDEX(3, 1)] = 0.0f;
	result.elements[INDEX(3, 2)] = 0.0f;
		
	result.elements[INDEX(3, 3)] = 1.0f;

	return result;
}


vec4 mat4::GetColumn(unsigned int col) const
{
	if (col > 3)
		return vec4();

	return vec4(*((vec4*)&elements[col * 4]));
}


mat4& mat4::Add(const mat4& matrix)
{
	elements[0] += matrix.elements[0];
	elements[1] += matrix.elements[1];
	elements[2] += matrix.elements[2];
	elements[3] += matrix.elements[3];

	elements[4] += matrix.elements[4];
	elements[5] += matrix.elements[5];
	elements[6] += matrix.elements[6];
	elements[7] += matrix.elements[7];

	elements[8] += matrix.elements[8];
	elements[9] += matrix.elements[9];
	elements[10] += matrix.elements[10];
	elements[11] += matrix.elements[11];

	elements[12] += matrix.elements[12];
	elements[13] += matrix.elements[13];
	elements[14] += matrix.elements[14];
	elements[15] += matrix.elements[15];

	return *this;
}

mat4& mat4::Subtract(const mat4& matrix)
{
	elements[0] -= matrix.elements[0];
	elements[1] -= matrix.elements[1];
	elements[2] -= matrix.elements[2];
	elements[3] -= matrix.elements[3];

	elements[4] -= matrix.elements[4];
	elements[5] -= matrix.elements[5];
	elements[6] -= matrix.elements[6];
	elements[7] -= matrix.elements[7];

	elements[8] -= matrix.elements[8];
	elements[9] -= matrix.elements[9];
	elements[10] -= matrix.elements[10];
	elements[11] -= matrix.elements[11];

	elements[12] -= matrix.elements[12];
	elements[13] -= matrix.elements[13];
	elements[14] -= matrix.elements[14];
	elements[15] -= matrix.elements[15];

	return *this;
}


mat4& mat4::Multiply(const mat4& matrix)
{
	float newElements[16];
	memset(newElements, 0, sizeof(newElements));

	unsigned int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			newElements[INDEX(i, j)] += elements[INDEX(i, 0)] * matrix.elements[INDEX(0, j)];
			newElements[INDEX(i, j)] += elements[INDEX(i, 1)] * matrix.elements[INDEX(1, j)];
			newElements[INDEX(i, j)] += elements[INDEX(i, 2)] * matrix.elements[INDEX(2, j)];
			newElements[INDEX(i, j)] += elements[INDEX(i, 3)] * matrix.elements[INDEX(3, j)];
		}
	}

	memcpy(elements, newElements, sizeof(elements));
	return *this;
}

mat4 mat4::Multiply(const mat4& left, const mat4& right)
{
	mat4 result;

	unsigned int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			result.elements[INDEX(i, j)] += left.elements[INDEX(i, 0)] * right.elements[INDEX(0, j)];
			result.elements[INDEX(i, j)] += left.elements[INDEX(i, 1)] * right.elements[INDEX(1, j)];
			result.elements[INDEX(i, j)] += left.elements[INDEX(i, 2)] * right.elements[INDEX(2, j)];
			result.elements[INDEX(i, j)] += left.elements[INDEX(i, 3)] * right.elements[INDEX(3, j)];
		}
	}

	return result;
}


mat4 operator*(const mat4& left, const mat4& right)
{
	return mat4::Multiply(left, right);
}


vec3 mat4::Multiply(const vec3& vec) const
{
	vec3 result;

	result.x = elements[INDEX(0, 0)] * vec.x
		+ elements[INDEX(0, 1)] * vec.y
		+ elements[INDEX(0, 2)] * vec.z
		+ elements[INDEX(0, 3)];

	result.y = elements[INDEX(1, 0)] * vec.x
		+ elements[INDEX(1, 1)] * vec.y
		+ elements[INDEX(1, 2)] * vec.z
		+ elements[INDEX(1, 3)];

	result.z = elements[INDEX(2, 0)] * vec.x
		+ elements[INDEX(2, 1)] * vec.y
		+ elements[INDEX(2, 2)] * vec.z
		+ elements[INDEX(2, 3)];

	return result;
}

vec3 mat4::RightMultiply(const vec3& vec) const
{
	vec3 result;

	result.x = elements[INDEX(0, 0)] * vec.x
		+ elements[INDEX(1, 0)] * vec.y
		+ elements[INDEX(2, 0)] * vec.z
		+ elements[INDEX(3, 0)];

	result.y = elements[INDEX(0, 1)] * vec.x
		+ elements[INDEX(1, 1)] * vec.y
		+ elements[INDEX(2, 1)] * vec.z
		+ elements[INDEX(3, 1)];

	result.z = elements[INDEX(0, 2)] * vec.x
		+ elements[INDEX(1, 2)] * vec.y
		+ elements[INDEX(2, 2)] * vec.z
		+ elements[INDEX(3, 2)];

	return result;
}

vec4 mat4::Multiply(const vec4& vec) const
{
	vec3 result;

	result.x = elements[INDEX(0, 0)] * vec.x
		+ elements[INDEX(0, 1)] * vec.y
		+ elements[INDEX(0, 2)] * vec.z
		+ elements[INDEX(0, 3)] * vec.w;

	result.y = elements[INDEX(1, 0)] * vec.x
		+ elements[INDEX(1, 1)] * vec.y
		+ elements[INDEX(1, 2)] * vec.z
		+ elements[INDEX(1, 3)] * vec.w;

	result.z = elements[INDEX(2, 0)] * vec.x
		+ elements[INDEX(2, 1)] * vec.y
		+ elements[INDEX(2, 2)] * vec.z
		+ elements[INDEX(2, 3)] * vec.w;

	result.w = elements[INDEX(3, 0)] * vec.x
		+ elements[INDEX(3, 1)] * vec.y
		+ elements[INDEX(3, 2)] * vec.z
		+ elements[INDEX(3, 3)] * vec.w;

	return result;
}

vec4 mat4::RightMultiply(const vec4& vec) const
{
	vec3 result;

	result.x = elements[INDEX(0, 0)] * vec.x
		+ elements[INDEX(1, 0)] * vec.y
		+ elements[INDEX(2, 0)] * vec.z
		+ elements[INDEX(3, 0)] * vec.w;

	result.y = elements[INDEX(0, 1)] * vec.x
		+ elements[INDEX(1, 1)] * vec.y
		+ elements[INDEX(2, 1)] * vec.z
		+ elements[INDEX(3, 1)] * vec.w;

	result.z = elements[INDEX(0, 2)] * vec.x
		+ elements[INDEX(1, 2)] * vec.y
		+ elements[INDEX(2, 2)] * vec.z
		+ elements[INDEX(3, 2)] * vec.w;

	result.w = elements[INDEX(0, 3)] * vec.x
		+ elements[INDEX(1, 3)] * vec.y
		+ elements[INDEX(2, 3)] * vec.z
		+ elements[INDEX(3, 3)] * vec.w;

	return result;
}


vec3 operator*(const mat4& matrix, const vec3& vec)
{
	return matrix.Multiply(vec);
}

vec3 operator*(const vec3& vec, const mat4& matrix)
{
	return matrix.RightMultiply(vec);
}

vec4 operator*(const mat4& matrix, const vec4& vec)
{
	return matrix.Multiply(vec);
}

vec4 operator*(const vec4& vec, const mat4& matrix)
{
	return matrix.RightMultiply(vec);
}


#define a11 elements[INDEX(0, 0)]
#define a12 elements[INDEX(0, 1)]
#define a13 elements[INDEX(0, 2)]
#define a14 elements[INDEX(0, 3)]
#define a21 elements[INDEX(1, 0)]
#define a22 elements[INDEX(1, 1)]
#define a23 elements[INDEX(1, 2)]
#define a24 elements[INDEX(1, 3)]
#define a31 elements[INDEX(2, 0)]
#define a32 elements[INDEX(2, 1)]
#define a33 elements[INDEX(2, 2)]
#define a34 elements[INDEX(2, 3)]
#define a41 elements[INDEX(3, 0)]
#define a42 elements[INDEX(3, 1)]
#define a43 elements[INDEX(3, 2)]
#define a44 elements[INDEX(3, 3)]

#define b11 newElements[INDEX(0, 0)]
#define b12 newElements[INDEX(0, 1)]
#define b13 newElements[INDEX(0, 2)]
#define b14 newElements[INDEX(0, 3)]
#define b21 newElements[INDEX(1, 0)]
#define b22 newElements[INDEX(1, 1)]
#define b23 newElements[INDEX(1, 2)]
#define b24 newElements[INDEX(1, 3)]
#define b31 newElements[INDEX(2, 0)]
#define b32 newElements[INDEX(2, 1)]
#define b33 newElements[INDEX(2, 2)]
#define b34 newElements[INDEX(2, 3)]
#define b41 newElements[INDEX(3, 0)]
#define b42 newElements[INDEX(3, 1)]
#define b43 newElements[INDEX(3, 2)]
#define b44 newElements[INDEX(3, 3)]

	float mat4::Determinant() const
	{
		float det = a11 * a22 * a33 * a44 - a11 * a22 * a34 * a43
			- a11 * a23 * a32 * a44 + a11 * a23 * a34 * a42
			+ a11 * a24 * a32 * a43 - a11 * a24 * a33 * a42
			- a12 * a21 * a33 * a44 + a12 * a21 * a34 * a43
			+ a12 * a23 * a31 * a44 - a12 * a23 * a34 * a41
			- a12 * a24 * a31 * a43 + a12 * a24 * a33 * a41
			+ a13 * a21 * a32 * a44 - a13 * a21 * a34 * a42
			- a13 * a22 * a31 * a44 + a13 * a22 * a34 * a41
			+ a13 * a24 * a31 * a42 - a13 * a24 * a32 * a41
			- a14 * a21 * a32 * a43 + a14 * a21 * a33 * a42
			+ a14 * a22 * a31 * a43 - a14 * a22 * a33 * a41
			- a14 * a23 * a31 * a42 + a14 * a23 * a32 * a41;

		return det;
	}


	mat4& mat4::Invert()
	{
		float newElements[16];

		b11 = a22 * a33 * a44 - a22 * a34 * a43 - a23 * a32 * a44
			+ a23 * a34 * a42 + a24 * a32 * a43 - a24 * a33 * a42;
		b12 = -(a12 * a33 * a44 - a12 * a34 * a43 - a13 * a32 * a44
			+ a13 * a34 * a42 + a14 * a32 * a43 - a14 * a33 * a42);
		b13 = a12 * a23 * a44 - a12 * a24 * a43 - a13 * a22 * a44
			+ a13 * a24 * a42 + a14 * a22 * a43 - a14 * a23 * a42;
		b14 = -(a12 * a23 * a34 - a12 * a24 * a33 - a13 * a22 * a34
			+ a13 * a24 * a32 + a14 * a22 * a33 - a14 * a23 * a32);

		b21 = -(a21 * a33 * a44 - a21 * a34 * a43 - a23 * a31 * a44
			+ a23 * a34 * a41 + a24 * a31 * a43 - a24 * a33 * a41);
		b22 = a11 * a33 * a44 - a11 * a34 * a43 - a13 * a31 * a44 
			+ a13 * a34 * a41 + a14 * a31 * a43 - a14 * a33 * a41;
		b23 = -(a11 * a23 * a44 - a11 * a24 * a43 - a13 * a21 * a44
			+ a13 * a24 * a41 + a14 * a21 * a43 - a14 * a23 * a41);
		b24 = a11 * a23 * a34 - a11 * a24 * a33 - a13 * a21 * a34
			+ a13 * a24 * a31 + a14 * a21 * a33 - a14 * a23 * a31;

		b31 = a21 * a32 * a44 - a21 * a34 * a42 - a22 * a31 * a44
			+ a22 * a34 * a41 + a24 * a31 * a42 - a24 * a32 * a41;
		b32 = -(a11 * a32 * a44 - a11 * a34 * a42 - a12 * a31 * a44
			+ a12 * a34 * a41 + a14 * a31 * a42 - a14 * a32 * a41);
		b33 = a11 * a22 * a44 - a11 * a24 * a42 - a12 * a21 * a44
			+ a12 * a24 * a41 + a14 * a21 * a42 - a14 * a22 * a41;
		b34 = -(a11 * a22 * a34 - a11 * a24 * a32 - a12 * a21 * a34
			+ a12 * a24 * a31 + a14 * a21 * a32 - a14 * a22 * a31);

		b41 = -(a21 * a32 * a43 - a21 * a33 * a42 - a22 * a31 * a43
			+ a22 * a33 * a41 + a23 * a31 * a42 - a23 * a32 * a41);
		b42 = a11 * a32 * a43 - a11 * a33 * a42 - a12 * a31 * a43
			+ a12 * a33 * a41 + a13 * a31 * a42 - a13 * a32 * a41;
		b43 = -(a11 * a22 * a43 - a11 * a23 * a42 - a12 * a21 * a43
			+ a12 * a23 * a41 + a13 * a21 * a42 - a13 * a22 * a41);
		b44 = a11 * a22 * a33 - a11 * a23 * a32 - a12 * a21 * a33
			+ a12 * a23 * a31 + a13 * a21 * a32 - a13 * a22 * a31;

		float detInv = 1.0f / Determinant();
		for (int i = 0; i < 16; i += 4)
		{
			newElements[i + 0] *= detInv;
			newElements[i + 1] *= detInv;
			newElements[i + 2] *= detInv;
			newElements[i + 3] *= detInv;
		}

		memcpy(elements, newElements, sizeof(elements));
		return *this;
	}

	mat4& mat4::Transpose()
	{
		float tmp;

		tmp = a12;
		a12 = a21;
		a21 = tmp;

		tmp = a13;
		a13 = a31;
		a31 = tmp;

		tmp = a14;
		a14 = a41;
		a41 = tmp;

		tmp = a23;
		a23 = a32;
		a32 = tmp;

		tmp = a24;
		a24 = a42;
		a42 = tmp;

		tmp = a34;
		a34 = a43;
		a43 = tmp;

		return *this;
	}

#pragma warning(disable: 4005)

#define a11 matrix.elements[INDEX(0, 0)]
#define a12 matrix.elements[INDEX(0, 1)]
#define a13 matrix.elements[INDEX(0, 2)]
#define a14 matrix.elements[INDEX(0, 3)]
#define a21 matrix.elements[INDEX(1, 0)]
#define a22 matrix.elements[INDEX(1, 1)]
#define a23 matrix.elements[INDEX(1, 2)]
#define a24 matrix.elements[INDEX(1, 3)]
#define a31 matrix.elements[INDEX(2, 0)]
#define a32 matrix.elements[INDEX(2, 1)]
#define a33 matrix.elements[INDEX(2, 2)]
#define a34 matrix.elements[INDEX(2, 3)]
#define a41 matrix.elements[INDEX(3, 0)]
#define a42 matrix.elements[INDEX(3, 1)]
#define a43 matrix.elements[INDEX(3, 2)]
#define a44 matrix.elements[INDEX(3, 3)]

#define b11 result.elements[INDEX(0, 0)]
#define b12 result.elements[INDEX(0, 1)]
#define b13 result.elements[INDEX(0, 2)]
#define b14 result.elements[INDEX(0, 3)]
#define b21 result.elements[INDEX(1, 0)]
#define b22 result.elements[INDEX(1, 1)]
#define b23 result.elements[INDEX(1, 2)]
#define b24 result.elements[INDEX(1, 3)]
#define b31 result.elements[INDEX(2, 0)]
#define b32 result.elements[INDEX(2, 1)]
#define b33 result.elements[INDEX(2, 2)]
#define b34 result.elements[INDEX(2, 3)]
#define b41 result.elements[INDEX(3, 0)]
#define b42 result.elements[INDEX(3, 1)]
#define b43 result.elements[INDEX(3, 2)]
#define b44 result.elements[INDEX(3, 3)]

#pragma warning(default: 4005)


mat4 mat4::Inverse(const mat4& matrix)
{
	mat4 result;

	b11 = a22 * a33 * a44 - a22 * a34 * a43 - a23 * a32 * a44
		+ a23 * a34 * a42 + a24 * a32 * a43 - a24 * a33 * a42;
	b12 = -(a12 * a33 * a44 - a12 * a34 * a43 - a13 * a32 * a44
		+ a13 * a34 * a42 + a14 * a32 * a43 - a14 * a33 * a42);
	b13 = a12 * a23 * a44 - a12 * a24 * a43 - a13 * a22 * a44
		+ a13 * a24 * a42 + a14 * a22 * a43 - a14 * a23 * a42;
	b14 = -(a12 * a23 * a34 - a12 * a24 * a33 - a13 * a22 * a34
		+ a13 * a24 * a32 + a14 * a22 * a33 - a14 * a23 * a32);

	b21 = -(a21 * a33 * a44 - a21 * a34 * a43 - a23 * a31 * a44
		+ a23 * a34 * a41 + a24 * a31 * a43 - a24 * a33 * a41);
	b22 = a11 * a33 * a44 - a11 * a34 * a43 - a13 * a31 * a44
		+ a13 * a34 * a41 + a14 * a31 * a43 - a14 * a33 * a41;
	b23 = -(a11 * a23 * a44 - a11 * a24 * a43 - a13 * a21 * a44
		+ a13 * a24 * a41 + a14 * a21 * a43 - a14 * a23 * a41);
	b24 = a11 * a23 * a34 - a11 * a24 * a33 - a13 * a21 * a34
		+ a13 * a24 * a31 + a14 * a21 * a33 - a14 * a23 * a31;

	b31 = a21 * a32 * a44 - a21 * a34 * a42 - a22 * a31 * a44
		+ a22 * a34 * a41 + a24 * a31 * a42 - a24 * a32 * a41;
	b32 = -(a11 * a32 * a44 - a11 * a34 * a42 - a12 * a31 * a44
		+ a12 * a34 * a41 + a14 * a31 * a42 - a14 * a32 * a41);
	b33 = a11 * a22 * a44 - a11 * a24 * a42 - a12 * a21 * a44
		+ a12 * a24 * a41 + a14 * a21 * a42 - a14 * a22 * a41;
	b34 = -(a11 * a22 * a34 - a11 * a24 * a32 - a12 * a21 * a34
		+ a12 * a24 * a31 + a14 * a21 * a32 - a14 * a22 * a31);

	b41 = -(a21 * a32 * a43 - a21 * a33 * a42 - a22 * a31 * a43
		+ a22 * a33 * a41 + a23 * a31 * a42 - a23 * a32 * a41);
	b42 = a11 * a32 * a43 - a11 * a33 * a42 - a12 * a31 * a43
		+ a12 * a33 * a41 + a13 * a31 * a42 - a13 * a32 * a41;
	b43 = -(a11 * a22 * a43 - a11 * a23 * a42 - a12 * a21 * a43
		+ a12 * a23 * a41 + a13 * a21 * a42 - a13 * a22 * a41);
	b44 = a11 * a22 * a33 - a11 * a23 * a32 - a12 * a21 * a33
		+ a12 * a23 * a31 + a13 * a21 * a32 - a13 * a22 * a31;

	float detInv = 1.0f / matrix.Determinant();
	for (int i = 0; i < 16; i += 4)
	{
		result.elements[i + 0] *= detInv;
		result.elements[i + 1] *= detInv;
		result.elements[i + 2] *= detInv;
		result.elements[i + 3] *= detInv;
	}

	return result;
}

mat4 mat4::Transpose(const mat4& matrix)
{
	mat4 result;

	b11 = a11;
	b22 = a22;
	b33 = a33;
	b44 = a44;

	b12 = a21;
	b21 = a12;

	b13 = a31;
	b31 = a13;

	b14 = a41;
	b41 = a14;

	b23 = a32;
	b32 = a23;

	b24 = a42;
	b42 = a24;

	b34 = a43;
	b43 = a34;

	return result;
}

#undef a11
#undef a12
#undef a13
#undef a14
#undef a21
#undef a22
#undef a23
#undef a24
#undef a31
#undef a32
#undef a33
#undef a34
#undef a41
#undef a42
#undef a43
#undef a44

#undef b11
#undef b12
#undef b13
#undef b14
#undef b21
#undef b22
#undef b23
#undef b24
#undef b31
#undef b32
#undef b33
#undef b34
#undef b41
#undef b42
#undef b43
#undef b44