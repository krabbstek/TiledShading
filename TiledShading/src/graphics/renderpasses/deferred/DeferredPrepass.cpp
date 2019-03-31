#include "DeferredPrepass.h"

#include "Globals.h"

#include <iostream>

DeferredPrepass::DeferredPrepass(Renderer& renderer, std::shared_ptr<GLShader> shader, std::shared_ptr<GLTexture2D> viewSpacePositionTexture, std::shared_ptr<GLTexture2D> viewSpaceNormalTexture)
	: RenderPass(renderer, shader), m_ViewSpacePositionTexture(viewSpacePositionTexture), m_ViewSpaceNormalTexture(viewSpaceNormalTexture)
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

DeferredPrepass::~DeferredPrepass()
{
	GLCall(glDeleteRenderbuffers(1, &m_PrepassDepthStencilRenderbuffer));
	m_PrepassDepthStencilRenderbuffer = 0;

	GLCall(glDeleteFramebuffers(1, &m_PrepassFramebuffer));
	m_PrepassFramebuffer = 0;
}


void DeferredPrepass::Render(std::vector<Renderable*>& renderables)
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_PrepassFramebuffer));
	GLCall(glViewport(0, 0, g_WindowWidth, g_WindowHeight));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	RenderPass::Render(renderables);
}