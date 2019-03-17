#include "GLShaderStorageBuffer.h"

#include "GLCommon.h"

GLShaderStorageBuffer::GLShaderStorageBuffer(const void* data, unsigned int size)
{
	GLCall(glGenBuffers(1, &m_RendererID));
	Bind();
	GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW));
}

GLShaderStorageBuffer::~GLShaderStorageBuffer()
{
	if (m_RendererID)
		GLCall(glDeleteBuffers(1, &m_RendererID));
}


void GLShaderStorageBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID));
}

void GLShaderStorageBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}