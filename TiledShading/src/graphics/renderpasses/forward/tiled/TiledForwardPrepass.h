#pragma once

#include "graphics/RenderPass.h"
#include "graphics/opengl/OpenGL.h"

class TiledForwardPrepass : public RenderPass
{
public:
	TiledForwardPrepass(Renderer& renderer, std::shared_ptr<GLShader> shader, std::shared_ptr<GLTexture2D> viewSpaceDepthTexture);
	~TiledForwardPrepass();

	virtual void Render(std::vector<Renderable*>& renderables) override;

	inline GLuint GetPrepassFramebuffer() const { return m_PrepassFramebuffer; }

protected:
	std::shared_ptr<GLTexture2D> m_ViewSpaceDepthTexture;
	GLuint m_PrepassFramebuffer;
	GLuint m_ForwardDepthRenderbuffer;
};