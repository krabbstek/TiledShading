#pragma once

#include "graphics/RenderPass.h"
#include "graphics/opengl/OpenGL.h"

class ClusteredDeferredPrepass : public RenderPass
{
public:
	ClusteredDeferredPrepass(Renderer& renderer, std::shared_ptr<GLShader> shader, std::shared_ptr<GLTexture2D> viewSpacePositionTexture, std::shared_ptr<GLTexture2D> viewSpaceNormalTexture);
	~ClusteredDeferredPrepass();

	virtual void Render(std::vector<Renderable*>& renderables) override;

protected:
	std::shared_ptr<GLTexture2D> m_ViewSpacePositionTexture;
	std::shared_ptr<GLTexture2D> m_ViewSpaceNormalTexture;
	GLuint m_PrepassFramebuffer;
	GLuint m_PrepassDepthRenderbuffer;
};