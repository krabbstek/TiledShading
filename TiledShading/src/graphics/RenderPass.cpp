#include "RenderPass.h"

void RenderPass::Render(std::vector<RenderableArgs>& renderables)
{
	for (RenderableArgs renderableArgs : renderables)
		renderableArgs.renderable->Render(*renderableArgs.renderer, *renderableArgs.shader);
}