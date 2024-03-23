#include "GLCommon.h"

#include <iostream>

void GLClearErrors()
{
	while (glGetError());
}

GLenum GLGetError()
{
	return glGetError();
}

bool GLLogError(const char* call, const char* file, int line)
{
	GLenum error = GLGetError();
	if (error != GL_NO_ERROR)
	{
		std::printf("OpenGL error {}: function {}, file {}, line {}", error, call, file, line);
		return false;
	}
	return true;
}
