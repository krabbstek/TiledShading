#pragma once

#include <glad/glad.h>

extern void GLClearErrors();
extern GLenum GLGetError();
extern bool GLLogError(const char* call, const char* file, int line);

#ifdef DEBUG
	#define GLCall(x) GLClearErrors();\
		x;\
		if (!GLLogError(#x, __FILE__, __LINE__)) __debugbreak()
#else
	#define GLCall(x) x
#endif