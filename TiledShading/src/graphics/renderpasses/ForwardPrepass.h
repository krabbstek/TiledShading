#pragma once

#include "../RenderPass.h"

class ForwardPrepass : public RenderPass
{
public:
	ForwardPrepass(std::shared_ptr<GLShader> shader)
		: m_Shader(shader) {}

	virtual void Render(std::vector<RenderableArgs>& renderables) override;

private:
	std::shared_ptr<GLShader> m_Shader;
};