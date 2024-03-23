#pragma once

#include "RenderPass.h"

class RenderTechnique
{
public:
	RenderTechnique();
	virtual ~RenderTechnique();

	virtual void AddRenderPass(std::shared_ptr<RenderPass> renderPass);

	virtual void Render(const Renderable& renderable, const Renderer& renderer, GLShader& shader);
	virtual void Render();

protected:
	std::vector<std::shared_ptr<RenderPass>> m_RenderPasses;
	std::vector<RenderableArgs> m_Renderables;
};