#include "TiledForwardLightingPass.h"

#include "Globals.h"

TiledForwardLightingPass::TiledForwardLightingPass(Renderer& renderer, std::shared_ptr<GLShader> shader, std::shared_ptr<GLTexture2D> viewSpaceDepthTexture, std::shared_ptr<GLShaderStorageBuffer> lightIndexSSBO, std::shared_ptr<GLShaderStorageBuffer> tileIndexSSBO, GLuint prepassFramebuffer, const Material& material)
	: RenderPass(renderer, shader),
	m_ViewSpaceDepthTexture(viewSpaceDepthTexture),
	m_LightSSBO(std::make_shared<GLShaderStorageBuffer>(&g_Lights, sizeof(g_Lights))),
	m_LightIndexSSBO(lightIndexSSBO),
	m_TileIndexSSBO(tileIndexSSBO),
	m_PrepassFramebuffer(prepassFramebuffer),
	m_Material(material)
{
	m_Shader->SetUniform1i("u_MaxNumLightsPerTile", g_MaxNumLightsPerTile);
}

TiledForwardLightingPass::~TiledForwardLightingPass()
{
}


void TiledForwardLightingPass::Render(std::vector<Renderable*>& renderables)
{
	GLCall(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_PrepassFramebuffer));
	GLCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
	GLCall(glBlitFramebuffer(0, 0, g_WindowWidth, g_WindowHeight, 0, 0, g_WindowWidth, g_WindowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST));

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glViewport(0, 0, g_WindowWidth, g_WindowHeight));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_Shader->Bind();
	m_LightSSBO->SetData(&g_Lights, sizeof(g_Lights));
	m_LightSSBO->Bind(3);
	m_LightIndexSSBO->Bind(4);
	m_TileIndexSSBO->Bind(5);

	m_Material.Bind(*m_Shader);

	RenderPass::Render(renderables);
}