#pragma once

#include "../../RenderPass.h"

#include "graphics/Material.h"
#include "graphics/opengl/OpenGL.h"
#include "meshes/FullscreenMesh.h"

class DeferredLightingPass : public RenderPass
{
public:
	DeferredLightingPass(Renderer& renderer, std::shared_ptr<GLShader> shader, std::shared_ptr<GLTexture2D> viewSpacePositionTexture, std::shared_ptr<GLTexture2D> viewSpaceNormalTexture, const Material& material);
	~DeferredLightingPass();

	virtual void Render(std::vector<Renderable*>&) override;

protected:
	std::shared_ptr<GLTexture2D> m_ViewSpacePositionTexture;
	std::shared_ptr<GLTexture2D> m_ViewSpaceNormalTexture;
	std::shared_ptr<GLShaderStorageBuffer> m_LightSSBO;
	const Material& m_Material;
	FullscreenMesh m_FullscreenMesh;
};