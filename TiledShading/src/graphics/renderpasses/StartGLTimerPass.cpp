#include "StartGLTimerPass.h"

StartGLTimerPass::StartGLTimerPass(Renderer& renderer, std::shared_ptr<GLTimer> timer)
	: RenderPass(renderer, std::shared_ptr<GLShader>()), m_Timer(timer)
{
}

void StartGLTimerPass::Render(std::vector<Renderable*>&)
{
	m_Timer->Start();
}