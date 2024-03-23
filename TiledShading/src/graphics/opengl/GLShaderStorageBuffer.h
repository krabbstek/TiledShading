#pragma once

#include <glad/glad.h>

class GLShaderStorageBuffer
{
public:
	GLShaderStorageBuffer(const void* data, unsigned int size);
	~GLShaderStorageBuffer();

	void Bind() const;
	void Unbind() const;

	inline GLuint RendererID() const { return m_RendererID; }
	inline unsigned int Size() const { return m_Size; }

private:
	GLuint m_RendererID;
	unsigned int m_Size;
};