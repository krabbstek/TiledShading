#pragma once

#include "CPUTimer.h"
#include "graphics/RenderPass.h"
#include "graphics/opengl/GLTimer.h"

class StopTimerPass : public RenderPass
{
public:
	StopTimerPass(Renderer& renderer, std::shared_ptr<Timer> timer);
	StopTimerPass(Renderer& renderer, std::shared_ptr<CPUTimer> timer);
	StopTimerPass(Renderer& renderer, std::shared_ptr<GLTimer> timer);
	~StopTimerPass() {}

	void Render(std::vector<Renderable*>&) override;

private:
	std::shared_ptr<Timer> m_Timer;
};
