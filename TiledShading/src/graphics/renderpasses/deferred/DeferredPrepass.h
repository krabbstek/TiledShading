#pragma once

#include "../../RenderPass.h"

#include "graphics/opengl/OpenGL.h"

class DeferredPrepass : public RenderPass
{
public:
	DeferredPrepass(Renderer& renderer, std::shared_ptr<GLShader> shader, std::shared_ptr<GLTexture2D> viewSpacePositionTexture, std::shared_ptr<GLTexture2D> viewSpaceNormalTexture);
	~DeferredPrepass();

	virtual void Render(std::vector<Renderable*>& renderables) override;

protected:
	std::shared_ptr<GLTexture2D> m_ViewSpacePositionTexture;
	std::shared_ptr<GLTexture2D> m_ViewSpaceNormalTexture;
	GLuint m_PrepassFramebuffer;
	GLuint m_PrepassDepthStencilRenderbuffer;
};