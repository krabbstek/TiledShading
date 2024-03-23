#pragma once

#include "graphics/Renderable.h"

#include "graphics/opengl/OpenGL.h"

class FullscreenMesh : public Renderable
{
public:
	FullscreenMesh();
	FullscreenMesh(std::shared_ptr<GLTexture2D> texture);
	FullscreenMesh(std::vector<std::shared_ptr<GLTexture2D>> textures);
	~FullscreenMesh();

	void AddTexture(std::shared_ptr<GLTexture2D> texture);

	void Render(const Renderer& renderer, GLShader& shader) const override;

private:
	std::vector<std::shared_ptr<GLTexture2D>> m_Textures;
	std::shared_ptr<GLVertexArray> m_VAO;
	std::shared_ptr<GLVertexBuffer> m_VBO;
	std::shared_ptr<GLIndexBuffer> m_IBO;
};