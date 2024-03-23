#include "RenderPass.h"

void RenderPass::Render(std::vector<Renderable*>& renderables)
{
	for (Renderable* renderable : renderables)
		renderable->Render(m_Renderer, *m_Shader);
}