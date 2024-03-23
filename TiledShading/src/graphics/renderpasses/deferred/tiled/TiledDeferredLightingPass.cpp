#include "TiledDeferredLightingPass.h"

#include "Globals.h"

TiledDeferredLightingPass::TiledDeferredLightingPass(Renderer& renderer, std::shared_ptr<GLShader> shader, std::shared_ptr<GLTexture2D> viewSpacePositionTexture, std::shared_ptr<GLTexture2D> viewSpaceNormalTexture, std::shared_ptr<GLShaderStorageBuffer> lightIndexSSBO, std::shared_ptr<GLShaderStorageBuffer> tileIndexSSBO, const Material& material)
	: RenderPass(renderer, shader),
	m_LightSSBO(std::make_shared<GLShaderStorageBuffer>(g_LightGrid, sizeof(g_LightGrid))),
	m_LightIndexSSBO(lightIndexSSBO),
	m_TileIndexSSBO(tileIndexSSBO),
	m_Material(material)
{
	m_FullscreenMesh.AddTexture(viewSpacePositionTexture);
	m_FullscreenMesh.AddTexture(viewSpaceNormalTexture);
}

TiledDeferredLightingPass::~TiledDeferredLightingPass()
{
}


void TiledDeferredLightingPass::Render(std::vector<Renderable*>&)
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glViewport(0, 0, g_WindowWidth, g_WindowHeight));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_Shader->Bind();
	m_Shader->SetUniform1f("u_LightFalloffThreshold", g_LightFalloffThreshold);
	m_LightSSBO->SetData(g_LightGrid, sizeof(g_LightGrid));
	m_LightSSBO->Bind(3);
	m_LightIndexSSBO->Bind(4);
	m_TileIndexSSBO->Bind(5);

	m_Material.Bind(*m_Shader);

	m_FullscreenMesh.Render(m_Renderer, *m_Shader);
}