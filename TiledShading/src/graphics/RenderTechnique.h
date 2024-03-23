#pragma once

#include "RenderPass.h"

class RenderTechnique
{
public:
	RenderTechnique();
	virtual ~RenderTechnique();

	virtual void AddRenderPass(std::shared_ptr<RenderPass> renderPass);

	virtual void Render(Renderable& renderable);
	virtual void Render();

protected:
	std::vector<std::shared_ptr<RenderPass>> m_RenderPasses;
	std::vector<Renderable*> m_Renderables;
};