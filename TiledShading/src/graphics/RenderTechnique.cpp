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


void RenderTechnique::Render(Renderable& renderable)
{
	m_Renderables.emplace_back(&renderable);
}

void RenderTechnique::Render()
{
	for (std::shared_ptr<RenderPass> renderPass : m_RenderPasses)
		renderPass->Render(m_Renderables);

	m_Renderables.clear();
}