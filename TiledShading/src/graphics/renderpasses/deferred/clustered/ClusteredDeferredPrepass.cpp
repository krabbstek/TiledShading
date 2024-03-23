#include "ClusteredDeferredPrepass.h"

#include "Globals.h"

#include <iostream>

ClusteredDeferredPrepass::ClusteredDeferredPrepass(Renderer& renderer, std::shared_ptr<GLShader> shader, std::shared_ptr<GLTexture2D> viewSpacePositionTexture, std::shared_ptr<GLTexture2D> viewSpaceNormalTexture)
	: RenderPass(renderer, shader),
	m_ViewSpacePositionTexture(viewSpacePositionTexture),
	m_ViewSpaceNormalTexture(viewSpaceNormalTexture)
{
	GLCall(glGenFramebuffers(1, &m_PrepassFramebuffer));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_PrepassFramebuffer));

	GLCall(glGenRenderbuffers(1, &m_PrepassDepthRenderbuffer));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_PrepassDepthRenderbuffer));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, g_WindowWidth, g_WindowHeight));
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_PrepassDepthRenderbuffer));

	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ViewSpacePositionTexture->RendererID(), 0));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_ViewSpaceNormalTexture->RendererID(), 0));
	GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	GLCall(glDrawBuffers(2, attachments));

	GLenum framebufferStatus;
	GLCall(framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		std::printf("Incomplete framebuffer!\n");
		__debugbreak();
	}
}

ClusteredDeferredPrepass::~ClusteredDeferredPrepass()
{
	GLCall(glDeleteRenderbuffers(1, &m_PrepassDepthRenderbuffer));
	m_PrepassDepthRenderbuffer = 0;

	GLCall(glDeleteFramebuffers(1, &m_PrepassFramebuffer));
	m_PrepassFramebuffer = 0;
}


void ClusteredDeferredPrepass::Render(std::vector<Renderable*>& renderables)
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_PrepassFramebuffer));
	GLCall(glViewport(0, 0, g_WindowWidth, g_WindowHeight));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_Shader->SetUniform1i("u_TileSize", g_TileSize);
	m_Shader->SetUniform1f("u_FarDepth", g_FarPlaneDepth);

	m_Shader->Bind();

	RenderPass::Render(renderables);
}