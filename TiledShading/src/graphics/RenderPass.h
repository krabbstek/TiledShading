#pragma once

#include "Renderable.h"

#include <vector>

class RenderPass
{
public:
	RenderPass(Renderer& renderer, std::shared_ptr<GLShader> shader)
		: m_Renderer(renderer), m_Shader(shader) {}
	virtual ~RenderPass() {}

	virtual void Render(std::vector<Renderable*>& renderables);

protected:
	Renderer& m_Renderer;
	std::shared_ptr<GLShader> m_Shader;
};