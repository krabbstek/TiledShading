#include "GLShaderStorageBuffer.h"

#include "GLCommon.h"

GLShaderStorageBuffer::GLShaderStorageBuffer(const void* data, unsigned int size)
{
	SetData(data, size);
}

GLShaderStorageBuffer::~GLShaderStorageBuffer()
{
	if (m_RendererID)
		GLCall(glDeleteBuffers(1, &m_RendererID));
}


void GLShaderStorageBuffer::SetData(const void* data, unsigned int size)
{
	if (!m_RendererID)
		GLCall(glGenBuffers(1, &m_RendererID));

	Bind();
	GLCall(glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW));
}


void GLShaderStorageBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID));
}

void GLShaderStorageBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}