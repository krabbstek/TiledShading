#pragma once

#include "graphics/RenderPass.h"
#include "graphics/Material.h"
#include "graphics/opengl/OpenGL.h"

#include "Globals.h"

class ForwardPass : public RenderPass
{
public:
	ForwardPass(Renderer& renderer, std::shared_ptr<GLShader> shader, const Material& material);

	virtual void Render(std::vector<Renderable*>& renderables) override;

private:
	std::shared_ptr<GLShaderStorageBuffer> m_LightSSBO;
	const Material& m_Material;
};