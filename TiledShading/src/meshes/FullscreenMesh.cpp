#include "FullscreenMesh.h"

static float vertices[] =
{
	-1.0f, -1.0f,
	 3.0f, -1.0f,
	-1.0f,  3.0f,
};

static unsigned int indices[] =
{
	0, 1, 2,
};

FullscreenMesh::FullscreenMesh()
{
	GLVertexBufferLayout layout;
	layout.Push(GL_FLOAT, 2);

	m_VBO = std::make_shared<GLVertexBuffer>(vertices, sizeof(vertices));
	m_VBO->SetVertexBufferLayout(layout);

	m_VAO = std::make_shared<GLVertexArray>();
	m_VAO->AddVertexBuffer(*m_VBO);

	m_IBO = std::make_shared<GLIndexBuffer>(indices, sizeof(indices) / sizeof(unsigned int));
}

FullscreenMesh::FullscreenMesh(std::shared_ptr<GLTexture2D> texture)
{
	GLVertexBufferLayout layout;
	layout.Push(GL_FLOAT, 2);

	m_VBO = std::make_shared<GLVertexBuffer>(vertices, sizeof(vertices));
	m_VBO->SetVertexBufferLayout(layout);

	m_VAO = std::make_shared<GLVertexArray>();
	m_VAO->AddVertexBuffer(*m_VBO);

	m_IBO = std::make_shared<GLIndexBuffer>(indices, sizeof(indices) / sizeof(unsigned int));

	m_Textures.push_back(texture);
}

FullscreenMesh::FullscreenMesh(std::vector<std::shared_ptr<GLTexture2D>> textures)
	: m_Textures(textures)
{
	GLVertexBufferLayout layout;
	layout.Push(GL_FLOAT, 2);

	m_VBO = std::make_shared<GLVertexBuffer>(vertices, sizeof(vertices));
	m_VBO->SetVertexBufferLayout(layout);

	m_VAO = std::make_shared<GLVertexArray>();
	m_VAO->AddVertexBuffer(*m_VBO);

	m_IBO = std::make_shared<GLIndexBuffer>(indices, sizeof(indices) / sizeof(unsigned int));
}

FullscreenMesh::~FullscreenMesh()
{
}


void FullscreenMesh::AddTexture(std::shared_ptr<GLTexture2D> texture)
{
	m_Textures.push_back(texture);
}


void FullscreenMesh::Render(const Renderer& renderer, GLShader& shader) const
{
	shader.Bind();
	m_VAO->Bind();
	m_IBO->Bind();
	size_t size = m_Textures.size();
	for (size_t i = 0; i < size; i++)
		m_Textures[i]->Bind(i);
	GLCall(glDrawElements(GL_TRIANGLES, m_IBO->Count(), GL_UNSIGNED_INT, 0));
}