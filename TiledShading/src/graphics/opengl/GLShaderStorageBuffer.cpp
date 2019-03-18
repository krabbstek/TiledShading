#include "GLShaderStorageBuffer.h"

#include "GLCommon.h"

GLShaderStorageBuffer::GLShaderStorageBuffer(const void* data, unsigned int size)
	: m_RendererID(0), m_Size(size)
{
	GLCall(glGenBuffers(1, &m_RendererID));
}

GLShaderStorageBuffer::~GLShaderStorageBuffer()
{
	if (m_RendererID)
	{
		GLCall(glDeleteBuffers(1, &m_RendererID));
	}
}


void GLShaderStorageBuffer::SetData(const void* data, unsigned int size)
{
	Bind();
	GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW));
	m_Size = size;
}


void GLShaderStorageBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID));
}

void GLShaderStorageBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}