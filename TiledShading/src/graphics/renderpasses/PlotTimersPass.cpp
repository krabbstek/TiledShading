#include "PlotTimersPass.h"

#include "imgui.h"
#include <sstream>

PlotTimersPass::PlotTimersPass(Renderer& renderer)
	: RenderPass(renderer, std::shared_ptr<GLShader>())
{
}


void PlotTimersPass::AddTimer(const char* label, std::shared_ptr<CPUTimer>& timer)
{
	m_Timers.emplace_back(timer);
	m_Labels.emplace_back(label);
}

void PlotTimersPass::AddTimer(const char* label, std::shared_ptr<GLTimer>& timer)
{
	m_Timers.emplace_back(timer);
	m_Labels.emplace_back(label);
}


void PlotTimersPass::Render(std::vector<Renderable*>&)
{
	ImGui::Begin("Performance");

	int numTimers = int(m_Timers.size());
	for (int i = 0; i < numTimers; i++)
	{
		m_Timers[i]->GetTime();
		PlotCyclicData(m_Labels[i], (float*)m_Timers[i]->GetData(), g_NumGraphSamples, m_Timers[i]->GetOffset());
	}

	ImGui::End();
}


void PlotTimersPass::PlotCyclicData(const char* label, const float* data, unsigned int dataLength, unsigned int offset)
{
	ImGui::Text(label);
	std::stringstream ss;
	ss << data[(offset - 1) % dataLength] << " ms";
	ImGui::PlotLines(ss.str().c_str(), [](void* data, int idx) { return ((float*)data)[idx]; }, (float*)data, dataLength, offset, NULL, 0, FLT_MAX, ImVec2(g_GraphSizeX, g_GraphSizeY));
}