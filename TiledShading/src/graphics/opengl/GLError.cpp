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
		std::printf("OpenGL error %d: function %s, file %s, line %d", error, call, file, line);
		return false;
	}
	return true;
}
