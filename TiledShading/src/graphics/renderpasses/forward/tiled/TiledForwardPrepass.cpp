#include "TiledForwardPrepass.h"

#include "Globals.h"

#include <iostream>

TiledForwardPrepass::TiledForwardPrepass(Renderer& renderer, std::shared_ptr<GLShader> shader, std::shared_ptr<GLTexture2D> viewSpaceDepthTexture)
	: RenderPass(renderer, shader),
	m_ViewSpaceDepthTexture(viewSpaceDepthTexture)
{
	GLCall(glGenFramebuffers(1, &m_PrepassFramebuffer));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_PrepassFramebuffer));

	GLCall(glGenRenderbuffers(1, &m_ForwardDepthRenderbuffer));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_ForwardDepthRenderbuffer));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, g_WindowWidth, g_WindowHeight));
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_ForwardDepthRenderbuffer));

	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ViewSpaceDepthTexture->RendererID(), 0));
	GLenum attachments = GL_COLOR_ATTACHMENT0;
	GLCall(glDrawBuffers(1, &attachments));

	GLenum status;
	GLCall(status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::printf("Framebuffer incomplete!\n");
		__debugbreak();
	}

	m_Shader->SetUniform1i("u_TileSize", g_TileSize);
	m_Shader->SetUniform1f("u_NearDepth", g_NearPlaneDepth);
	m_Shader->SetUniform1f("u_FarDepth", g_FarPlaneDepth);
}

TiledForwardPrepass::~TiledForwardPrepass()
{
	GLCall(glDeleteRenderbuffers(1, &m_ForwardDepthRenderbuffer));
	m_ForwardDepthRenderbuffer = 0;

	GLCall(glDeleteFramebuffers(1, &m_PrepassFramebuffer));
	m_PrepassFramebuffer = 0;
}


void TiledForwardPrepass::Render(std::vector<Renderable*>& renderables)
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_PrepassFramebuffer));
	GLCall(glViewport(0, 0, g_WindowWidth, g_WindowHeight));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	RenderPass::Render(renderables);
}