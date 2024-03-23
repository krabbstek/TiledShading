#pragma once

#include "Globals.h"
#include "CPUTimer.h"
#include "graphics/RenderPass.h"
#include "graphics/opengl/GLTimer.h"

class PlotTimersPass : public RenderPass
{
public:
	PlotTimersPass(Renderer& renderer);
	~PlotTimersPass() {}

	void AddLargeTimer(const char* label, std::shared_ptr<Timer>& timer);
	void AddLargeTimer(const char* label, std::shared_ptr<CPUTimer>& timer);
	void AddLargeTimer(const char* label, std::shared_ptr<GLTimer>& timer);

	void AddSmallTimer(const char* label, std::shared_ptr<Timer>& timer);
	void AddSmallTimer(const char* label, std::shared_ptr<CPUTimer>& timer);
	void AddSmallTimer(const char* label, std::shared_ptr<GLTimer>& timer);

	void Render(std::vector<Renderable*>&) override;

	static void PlotCyclicData(const char* label, const float* data, unsigned int dataLength, unsigned int offset, int graphSizeX, int graphSizeY);

private:
	std::vector<std::shared_ptr<Timer>> m_Timers;
	std::vector<const char*> m_Labels;
	std::vector<int> m_GraphSizeX;
	std::vector<int> m_GraphSizeY;
};