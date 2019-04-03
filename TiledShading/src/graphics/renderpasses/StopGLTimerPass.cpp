#include "StopGLTimerPass.h"

StopGLTimerPass::StopGLTimerPass(Renderer& renderer, std::shared_ptr<GLTimer> timer)
	: RenderPass(renderer, std::shared_ptr<GLShader>()), m_Timer(timer)
{
}

void StopGLTimerPass::Render(std::vector<Renderable*>&)
{
	m_Timer->Stop();
}