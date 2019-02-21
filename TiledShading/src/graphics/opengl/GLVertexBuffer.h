#pragma once

#include "GLVertexBufferLayout.h"

#include <glad/glad.h>

class GLVertexBuffer
{
public:
	GLVertexBuffer(const void* data, unsigned int size);
	~GLVertexBuffer();

	inline void SetVertexBufferLayout(const GLVertexBufferLayout& layout) { m_Layout = layout; }
	inline const GLVertexBufferLayout& GetVertexBufferLayout() const { return m_Layout; }

	void Bind() const;
	static void Unbind();

private:
	GLuint m_RendererID;
	GLVertexBufferLayout m_Layout;
};