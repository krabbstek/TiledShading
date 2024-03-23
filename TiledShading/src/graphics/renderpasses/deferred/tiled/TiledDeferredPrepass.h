#pragma once

#include "graphics/RenderPass.h"
#include "graphics/opengl/OpenGL.h"

class TiledDeferredPrepass : public RenderPass
{
public:
	TiledDeferredPrepass(Renderer& renderer, std::shared_ptr<GLShader> shader, std::shared_ptr<GLTexture2D> viewSpacePositionTexture, std::shared_ptr<GLTexture2D> viewSpaceNormalTexture, std::shared_ptr<GLImageTexture2D> tileMinImageTexture, std::shared_ptr<GLImageTexture2D> tileMaxImageTexture);
	~TiledDeferredPrepass();

	virtual void Render(std::vector<Renderable*>& renderables) override;

protected:
	std::shared_ptr<GLTexture2D> m_ViewSpacePositionTexture;
	std::shared_ptr<GLTexture2D> m_ViewSpaceNormalTexture;
	std::shared_ptr<GLImageTexture2D> m_TileMinImageTexture;
	std::shared_ptr<GLImageTexture2D> m_TileMaxImageTexture;
	GLuint m_PrepassFramebuffer;
	GLuint m_PrepassDepthStencilRenderbuffer;
};