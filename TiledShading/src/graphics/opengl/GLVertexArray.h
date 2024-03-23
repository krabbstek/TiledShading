#pragma once

#include "GLVertexBuffer.h"

#include <glad/glad.h>

class GLVertexArray
{
public:
	GLVertexArray();
	~GLVertexArray();

	void AddVertexBuffer(const GLVertexBuffer& vbo);

	void Bind() const;
	static void Unbind();

private:
	GLuint m_RendererID;
	unsigned int m_NumLocations;
};
