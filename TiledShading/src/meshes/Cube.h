#pragma once

#include "graphics/Renderable.h"

#include "graphics/opengl/OpenGL.h"

class Cube : public Renderable
{
public:
	vec3 position;

	Cube(const vec3& position = { 0.0f, 0.0f, 0.0f });
	~Cube();

	void Render(const Renderer& renderer, GLShader& shader) const override;

private:
	GLVertexArray* m_VAO;
	GLVertexBuffer* m_VBO;
	GLIndexBuffer* m_IBO;
};