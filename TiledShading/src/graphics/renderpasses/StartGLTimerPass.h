#pragma once

#include "../opengl/GLTimer.h"
#include "../RenderPass.h"

class StartGLTimerPass : public RenderPass
{
public:
	StartGLTimerPass(Renderer& renderer, std::shared_ptr<GLTimer> timer);
	~StartGLTimerPass() {}

	void Render(std::vector<Renderable*>&) override;

private:
	std::shared_ptr<GLTimer> m_Timer;
};