#pragma once

#include "graphics/Renderer.h"

#include "graphics/opengl/OpenGL.h"

class Cube
{
public:
	vec3 position;

	Cube(const vec3& position = { 0.0f, 0.0f, 0.0f }, const vec3& color = { 1.0f, 1.0f, 1.0f });
	~Cube();

	void Render(Renderer& renderer, GLShader& shader);

private:
	GLVertexArray* m_VAO;
	GLVertexBuffer* m_VBO;
	GLIndexBuffer* m_IBO;
};