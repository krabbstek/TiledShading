#include "Cube.h"

float cubeVertices[] =
{
	// Position | Normal

	// Left
	-0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,

	// Right
	 0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,

	 // Bottom
	-0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,

	// Top
	-0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,

	// Back
	-0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,

	// Front
	-0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
};

unsigned int cubeIndices[] =
{
	// Left
	 0,  1,  2,
	 0,  2,  3,

	// Right
	 4,  5,  6,
	 4,  6,  7,

	// Bottom
	 8,  9, 10,
	 8, 10, 11,

	// Top
	12, 13, 14,
	12, 14, 15,

	// Back
	16, 17, 18,
	16, 18, 19,

	// Front
	20, 21, 22,
	20, 22, 23,
};

Cube::Cube(const vec3& position /*= { 0.0f, 0.0f, 0.0f }*/)
	: position(position)
{
	m_VBO = new GLVertexBuffer(cubeVertices, sizeof(cubeVertices));
	GLVertexBufferLayout layout;
	layout.Push(GL_FLOAT, 3);
	layout.Push(GL_FLOAT, 3);
	m_VBO->SetVertexBufferLayout(layout);

	m_VAO = new GLVertexArray();
	m_VAO->AddVertexBuffer(*m_VBO);
	
	m_IBO = new GLIndexBuffer(cubeIndices, sizeof(cubeIndices));
}

Cube::~Cube()
{
	delete m_VAO;
	delete m_VBO;
	delete m_IBO;
}


void Cube::Render(const Renderer& renderer, GLShader& shader) const
{
	shader.Bind();
	mat4 M = mat4::Translate(position);
	mat4 V = renderer.camera.GetViewMatrix();
	mat4 P = renderer.camera.projectionMatrix;
	mat4 MV = V * M;
	shader.SetUniformMat4("u_MV", MV);
	shader.SetUniformMat4("u_MVP", P * MV);
	shader.SetUniformMat4("u_MV_normal", mat4::Transpose(mat4::Inverse(MV)));

	m_VAO->Bind();
	m_IBO->Bind();
	GLCall(glDrawElements(GL_TRIANGLES, m_IBO->Count(), GL_UNSIGNED_INT, 0));
}