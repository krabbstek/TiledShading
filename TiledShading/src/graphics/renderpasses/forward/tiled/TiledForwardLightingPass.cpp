#include "TiledForwardLightingPass.h"

#include "Globals.h"

TiledForwardLightingPass::TiledForwardLightingPass(Renderer& renderer, std::shared_ptr<GLShader> shader, std::shared_ptr<GLTexture2D> viewSpaceDepthTexture, std::shared_ptr<GLShaderStorageBuffer> lightIndexSSBO, std::shared_ptr<GLShaderStorageBuffer> tileIndexSSBO, GLuint forwardDepthRenderbuffer, const Material& material)
	: RenderPass(renderer, shader),
	m_ViewSpaceDepthTexture(viewSpaceDepthTexture),
	m_LightSSBO(std::make_shared<GLShaderStorageBuffer>(&g_Lights, sizeof(g_Lights))),
	m_LightIndexSSBO(lightIndexSSBO),
	m_TileIndexSSBO(tileIndexSSBO),
	m_Material(material)
{
	GLCall(glGenFramebuffers(1, &m_LightingPassFramebuffer));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_LightingPassFramebuffer));

	GLCall(glGenRenderbuffers(1, &m_LightingPassColorRenderbuffer));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_LightingPassColorRenderbuffer));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA32F, g_WindowWidth, g_WindowHeight));
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_LightingPassColorRenderbuffer));

	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, forwardDepthRenderbuffer));
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, forwardDepthRenderbuffer));

	GLenum attachments = GL_COLOR_ATTACHMENT0;
	GLCall(glDrawBuffers(1, &attachments));

	GLenum status;
	GLCall(status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::printf("Framebuffer incomplete!\n");
		__debugbreak();
	}

	m_Shader->SetUniform1i("u_MaxNumLightsPerTile", g_MaxNumLightsPerTile);
}

TiledForwardLightingPass::~TiledForwardLightingPass()
{
	GLCall(glDeleteRenderbuffers(1, &m_LightingPassColorRenderbuffer));
	m_LightingPassColorRenderbuffer = 0;

	GLCall(glDeleteFramebuffers(1, &m_LightingPassFramebuffer));
	m_LightingPassFramebuffer = 0;
}


void TiledForwardLightingPass::Render(std::vector<Renderable*>& renderables)
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_LightingPassFramebuffer));
	GLCall(glViewport(0, 0, g_WindowWidth, g_WindowHeight));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));

	m_Shader->Bind();
	m_LightSSBO->SetData(&g_Lights, sizeof(g_Lights));
	m_LightSSBO->Bind(3);
	m_LightIndexSSBO->Bind(4);
	m_TileIndexSSBO->Bind(5);

	m_Material.Bind(*m_Shader);

	RenderPass::Render(renderables);

	GLCall(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_LightingPassFramebuffer));
	GLCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
	GLCall(glBlitFramebuffer(0, 0, g_WindowWidth, g_WindowHeight, 0, 0, g_WindowWidth, g_WindowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST));
}