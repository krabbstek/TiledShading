#include "PlotTimersPass.h"

#include "imgui.h"
#include <sstream>

PlotTimersPass::PlotTimersPass(Renderer& renderer)
	: RenderPass(renderer, std::shared_ptr<GLShader>())
{
}


void PlotTimersPass::AddTimer(const char* label, std::shared_ptr<GLTimer>& timer)
{
	m_Timers.emplace_back(timer);
	m_Data.emplace_back(new float[g_NumGraphSamples], [](float* ptr) {delete[] ptr; });
	m_Labels.emplace_back(label);
}


void PlotTimersPass::Render(std::vector<Renderable*>&)
{
	int numTimers = m_Timers.size();
	for (int i = 0; i < numTimers; i++)
	{
		float* data = &(*m_Data[i]);
		data[g_PlotOffset] = float(m_Timers[i]->GetTime()) * 1e-6f;
		PlotCyclicData(m_Labels[i], data, g_NumGraphSamples, g_PlotOffset);
	}
}


void PlotTimersPass::PlotCyclicData(const char* label, float* data, unsigned int dataLength, unsigned int offset)
{
	ImGui::Text(label);
	std::stringstream ss;
	ss << data[(offset - 1) % dataLength] << " ms";
	ImGui::PlotLines(ss.str().c_str(), [](void* data, int idx) { return ((float*)data)[idx]; }, data, dataLength, offset, NULL, 0, FLT_MAX, ImVec2(g_GraphSizeX, g_GraphSizeY));
}