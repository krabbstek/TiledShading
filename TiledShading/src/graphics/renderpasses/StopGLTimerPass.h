#pragma once

#include "../opengl/GLTimer.h"
#include "../RenderPass.h"

class StopGLTimerPass : public RenderPass
{
public:
	StopGLTimerPass(Renderer& renderer, std::shared_ptr<GLTimer> timer);
	~StopGLTimerPass() {}

	void Render(std::vector<Renderable*>&) override;

private:
	std::shared_ptr<GLTimer> m_Timer;
};