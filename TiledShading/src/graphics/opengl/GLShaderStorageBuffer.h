#pragma once

#include <glad/glad.h>

class GLShaderStorageBuffer
{
public:
	GLShaderStorageBuffer(const void* data, unsigned int size);
	~GLShaderStorageBuffer();

	void SetData(const void* data, unsigned int size);

	void Bind() const;
	void Bind(unsigned int binding) const;
	void Unbind() const;

	inline GLuint RendererID() const { return m_RendererID; }
	inline unsigned int Size() const { return m_Size; }

private:
	GLuint m_RendererID;
	unsigned int m_Size;
};