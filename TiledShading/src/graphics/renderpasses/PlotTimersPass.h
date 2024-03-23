#pragma once

#include "Globals.h"

#include "../opengl/GLTimer.h"
#include "../RenderPass.h"

class PlotTimersPass : public RenderPass
{
public:
	PlotTimersPass(Renderer& renderer);
	~PlotTimersPass() {}

	void AddTimer(const char* label, std::shared_ptr<GLTimer>& timer);

	void Render(std::vector<Renderable*>&) override;

	static void PlotCyclicData(const char* label, const float* data, unsigned int dataLength, unsigned int offset);

private:
	std::vector<std::shared_ptr<GLTimer>> m_Timers;
	std::vector<const char*> m_Labels;
};