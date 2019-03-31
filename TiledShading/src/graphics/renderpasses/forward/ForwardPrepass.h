#pragma once

#include "../../RenderPass.h"

class ForwardPrepass : public RenderPass
{
public:
	ForwardPrepass(Renderer& renderer, std::shared_ptr<GLShader> shader)
		: RenderPass(renderer, shader) {}

	virtual void Render(std::vector<Renderable*>& renderables) override;
};