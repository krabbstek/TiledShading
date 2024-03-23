#include "RenderTechnique.h"

RenderTechnique::RenderTechnique()
{
}

RenderTechnique::~RenderTechnique()
{
}


void RenderTechnique::AddRenderPass(std::shared_ptr<RenderPass> renderPass)
{
	m_RenderPasses.emplace_back(renderPass);
}


void RenderTechnique::Render(const Renderable& renderable, const Renderer& renderer, GLShader& shader)
{
	m_Renderables.emplace_back(&renderable, &renderer, &shader);
}

void RenderTechnique::Render()
{
	for (std::shared_ptr<RenderPass> renderPass : m_RenderPasses)
		renderPass->Render(m_Renderables);
}