#include "TiledDeferredLightingPass.h"

#include "Globals.h"

TiledDeferredLightingPass::TiledDeferredLightingPass(Renderer& renderer, std::shared_ptr<GLShader> shader, std::shared_ptr<GLTexture2D> viewSpacePositionTexture, std::shared_ptr<GLTexture2D> viewSpaceNormalTexture, std::shared_ptr<GLShaderStorageBuffer> lightIndexSSBO, std::shared_ptr<GLShaderStorageBuffer> tileIndexSSBO, const Material& material)
	: RenderPass(renderer, shader),
	m_ViewSpacePositionTexture(viewSpacePositionTexture),
	m_ViewSpaceNormalTexture(viewSpaceNormalTexture),
	m_LightSSBO(std::make_shared<GLShaderStorageBuffer>(&g_Lights, sizeof(g_Lights))),
	m_LightIndexSSBO(lightIndexSSBO),
	m_TileIndexSSBO(tileIndexSSBO),
	m_Material(material)
{
	m_FullscreenMesh.AddTexture(viewSpacePositionTexture);
	m_FullscreenMesh.AddTexture(viewSpaceNormalTexture);

	m_Shader->SetUniform1i("u_MaxNumLightsPerTile", g_MaxNumLightsPerTile);
	m_Shader->SetUniform1i("u_TileSize", g_TileSize);
}

TiledDeferredLightingPass::~TiledDeferredLightingPass()
{
}


void TiledDeferredLightingPass::Render(std::vector<Renderable*>&)
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glViewport(0, 0, g_WindowWidth, g_WindowHeight));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_ViewSpacePositionTexture->Bind(0);
	m_ViewSpacePositionTexture->Bind(1);

	m_Shader->Bind();
	m_Shader->SetUniform1f("u_HeatmapAlpha", g_HeatmapAlpha);

	m_LightSSBO->SetData(&g_Lights, sizeof(g_Lights));
	m_LightSSBO->Bind(3);
	m_LightIndexSSBO->Bind(4);
	m_TileIndexSSBO->Bind(5);

	m_Material.Bind(*m_Shader);

	m_FullscreenMesh.Render(m_Renderer, *m_Shader);
}