#include "DeferredLightingPass.h"

#include "Globals.h"

DeferredLightingPass::DeferredLightingPass(Renderer& renderer, std::shared_ptr<GLShader> shader, std::shared_ptr<GLTexture2D> viewSpacePositionTexture, std::shared_ptr<GLTexture2D> viewSpaceNormalTexture, const Material& material)
	: RenderPass(renderer, shader),
	m_LightSSBO(std::make_shared<GLShaderStorageBuffer>(&g_Lights, sizeof(g_Lights))),
	m_Material(material)
{
	m_FullscreenMesh.AddTexture(viewSpacePositionTexture);
	m_FullscreenMesh.AddTexture(viewSpaceNormalTexture);
}

DeferredLightingPass::~DeferredLightingPass()
{
}


void DeferredLightingPass::Render(std::vector<Renderable*>&)
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glViewport(0, 0, g_WindowWidth, g_WindowHeight));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_Shader->Bind();
	m_LightSSBO->SetData(&g_Lights, sizeof(g_Lights));
	m_LightSSBO->Bind(3);

	m_Material.Bind(*m_Shader);

	m_FullscreenMesh.Render(m_Renderer, *m_Shader);
}