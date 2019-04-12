#pragma once

#include "graphics/Material.h"
#include "graphics/RenderPass.h"
#include "graphics/opengl/OpenGL.h"
#include "meshes/FullscreenMesh.h"

class TiledDeferredLightingPass : public RenderPass
{
public:
	TiledDeferredLightingPass(
		Renderer& renderer,
		std::shared_ptr<GLShader> shader,
		std::shared_ptr<GLTexture2D> viewSpacePositionTexture,
		std::shared_ptr<GLTexture2D> viewSpaceNormalTexture,
		std::shared_ptr<GLShaderStorageBuffer> lightIndexSSBO,
		const Material& material);
	~TiledDeferredLightingPass();

	virtual void Render(std::vector<Renderable*>&) override;

protected:
	std::shared_ptr<GLTexture2D> m_ViewSpacePositionTexture;
	std::shared_ptr<GLTexture2D> m_ViewSpaceNormalTexture;
	std::shared_ptr<GLShaderStorageBuffer> m_LightSSBO;
	std::shared_ptr<GLShaderStorageBuffer> m_LightIndexSSBO;
	const Material& m_Material;
	FullscreenMesh m_FullscreenMesh;
};