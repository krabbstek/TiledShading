#include "TiledDeferredPrepass.h"

#include "Globals.h"

#include <iostream>

TiledDeferredPrepass::TiledDeferredPrepass(Renderer& renderer, std::shared_ptr<GLShader> shader, std::shared_ptr<GLTexture2D> viewSpacePositionTexture, std::shared_ptr<GLTexture2D> viewSpaceNormalTexture, std::shared_ptr<GLImageTexture2D> tileMinImageTexture, std::shared_ptr<GLImageTexture2D> tileMaxImageTexture)
	: RenderPass(renderer, shader),
	m_ViewSpacePositionTexture(viewSpacePositionTexture),
	m_ViewSpaceNormalTexture(viewSpaceNormalTexture),
	m_TileMinImageTexture(tileMinImageTexture),
	m_TileMaxImageTexture(tileMaxImageTexture)
{
	GLCall(glGenFramebuffers(1, &m_PrepassFramebuffer));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_PrepassFramebuffer));

	GLCall(glGenRenderbuffers(1, &m_PrepassDepthStencilRenderbuffer));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_PrepassDepthStencilRenderbuffer));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, g_WindowWidth, g_WindowHeight));
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_PrepassDepthStencilRenderbuffer));

	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ViewSpacePositionTexture->RendererID(), 0));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_ViewSpaceNormalTexture->RendererID(), 0));
	GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	GLCall(glDrawBuffers(2, attachments));
}

TiledDeferredPrepass::~TiledDeferredPrepass()
{
	GLCall(glDeleteRenderbuffers(1, &m_PrepassDepthStencilRenderbuffer));
	m_PrepassDepthStencilRenderbuffer = 0;

	GLCall(glDeleteFramebuffers(1, &m_PrepassFramebuffer));
	m_PrepassFramebuffer = 0;
}


void TiledDeferredPrepass::Render(std::vector<Renderable*>& renderables)
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_PrepassFramebuffer));
	GLCall(glViewport(0, 0, g_WindowWidth, g_WindowHeight));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_Shader->SetUniform1i("u_TileSize", g_TileSize);
	m_Shader->SetUniform1f("u_FarDepth", g_FarPlaneDepth);

	m_Shader->Bind();
	m_TileMinImageTexture->Bind(3);
	m_TileMaxImageTexture->Bind(4);

	RenderPass::Render(renderables);

	int pixels[g_WindowWidth * g_WindowHeight / (g_TileSize * g_TileSize)];
	float pixels_f[g_WindowWidth * g_WindowHeight / (g_TileSize * g_TileSize)];

	GLCall(glBindTexture(GL_TEXTURE_2D, m_TileMaxImageTexture->RendererID()));
	GLCall(glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_INT, pixels));

	for (int i = 0; i < (sizeof(pixels) / sizeof(int)); i++)
		pixels_f[i] = float(pixels[i]) * (g_FarPlaneDepth / float(0x7FFFFFFF));

	std::printf("%p\n", glGetTextureImage);
}