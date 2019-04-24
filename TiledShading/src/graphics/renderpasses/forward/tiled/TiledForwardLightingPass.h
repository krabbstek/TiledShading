#pragma once

#include "graphics/Material.h"
#include "graphics/RenderPass.h"
#include "graphics/opengl/OpenGL.h"
#include "meshes/FullscreenMesh.h"

class TiledForwardLightingPass : public RenderPass
{
public:
	TiledForwardLightingPass(
		Renderer& renderer,
		std::shared_ptr<GLShader> shader,
		std::shared_ptr<GLTexture2D> viewSpaceDepthTexture,
		std::shared_ptr<GLShaderStorageBuffer> lightIndexSSBO,
		std::shared_ptr<GLShaderStorageBuffer> tileIndexSSBO,
		GLuint forwardDepthRenderbuffer,
		const Material& material);
	~TiledForwardLightingPass();

	virtual void Render(std::vector<Renderable*>&) override;

protected:
	std::shared_ptr<GLTexture2D> m_ViewSpaceDepthTexture;
	std::shared_ptr<GLShaderStorageBuffer> m_LightSSBO;
	std::shared_ptr<GLShaderStorageBuffer> m_LightIndexSSBO;
	std::shared_ptr<GLShaderStorageBuffer> m_TileIndexSSBO;
	GLuint m_LightingPassFramebuffer;
	GLuint m_LightingPassColorRenderbuffer;
	const Material& m_Material;
};