#pragma once

#include "graphics/RenderPass.h"

class ClearDefaultFramebufferPass : public RenderPass
{
public:
	ClearDefaultFramebufferPass(Renderer& renderer, std::shared_ptr<GLShader> shader)
		: RenderPass(renderer, shader) {}

	virtual void Render(std::vector<Renderable*>& renderables) override;
};