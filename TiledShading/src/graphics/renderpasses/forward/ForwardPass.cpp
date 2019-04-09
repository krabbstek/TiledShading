#include "ForwardPass.h"

#include "Globals.h"
#include "graphics/opengl/OpenGL.h"

ForwardPass::ForwardPass(Renderer& renderer, std::shared_ptr<GLShader> shader, const Material& material)
	: RenderPass(renderer, shader),
	m_LightSSBO(std::make_shared<GLShaderStorageBuffer>(g_LightGrid, sizeof(g_LightGrid))),
	m_Material(material)
{
}


void ForwardPass::Render(std::vector<Renderable*>& renderables)
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glViewport(0, 0, g_WindowWidth, g_WindowHeight));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_Material.Bind(*m_Shader);
	m_Shader->SetUniform1f("u_LightFalloffThreshold", g_LightFalloffThreshold);
	m_LightSSBO->SetData(g_LightGrid, sizeof(g_LightGrid));
	m_LightSSBO->Bind(3);

	RenderPass::Render(renderables);
}