#include "StopTimerPass.h"

StopTimerPass::StopTimerPass(Renderer& renderer, std::shared_ptr<Timer> timer)
	: RenderPass(renderer, std::shared_ptr<GLShader>()), m_Timer(timer)
{
}

StopTimerPass::StopTimerPass(Renderer& renderer, std::shared_ptr<CPUTimer> timer)
	: RenderPass(renderer, std::shared_ptr<GLShader>()), m_Timer(timer)
{
}

StopTimerPass::StopTimerPass(Renderer& renderer, std::shared_ptr<GLTimer> timer)
	: RenderPass(renderer, std::shared_ptr<GLShader>()), m_Timer(timer)
{
}

void StopTimerPass::Render(std::vector<Renderable*>&)
{
	m_Timer->Stop();
}