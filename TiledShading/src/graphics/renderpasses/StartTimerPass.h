#pragma once

#include "CPUTimer.h"
#include "graphics/RenderPass.h"
#include "graphics/opengl/GLTimer.h"

class StartTimerPass : public RenderPass
{
public:
	StartTimerPass(Renderer& renderer, std::shared_ptr<CPUTimer> timer);
	StartTimerPass(Renderer& renderer, std::shared_ptr<GLTimer> timer);
	~StartTimerPass() {}

	void Render(std::vector<Renderable*>&) override;

private:
	std::shared_ptr<Timer> m_Timer;
};