#include "GLVertexBufferLayout.h"

#include "GLCommon.h"

GLVertexBufferLayout::GLVertexBufferLayout()
	: m_Stride(0)
{
}


void GLVertexBufferLayout::Push(GLuint type, unsigned int count)
{
	m_Layout.push_back(LayoutElement(type, count));
	m_Stride += count * GetElementSize(type);
}


unsigned int GLVertexBufferLayout::GetElementSize(GLuint type)
{
	switch (type)
	{
	case GL_UNSIGNED_BYTE:
		return 1;

	case GL_FLOAT:
	case GL_INT:
	case GL_UNSIGNED_INT:
		return 4;

	default:
		return 0;
	}
}