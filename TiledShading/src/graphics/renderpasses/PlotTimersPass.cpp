#include "PlotTimersPass.h"

#include "imgui.h"
#include <sstream>

PlotTimersPass::PlotTimersPass(Renderer& renderer)
	: RenderPass(renderer, std::shared_ptr<GLShader>())
{
}


void PlotTimersPass::AddLargeTimer(const char* label, std::shared_ptr<Timer>& timer)
{
	m_Timers.emplace_back(timer);
	m_Labels.emplace_back(label);
	m_GraphSizeX.emplace_back(g_LargeGraphSizeX);
	m_GraphSizeY.emplace_back(g_LargeGraphSizeY);
}

void PlotTimersPass::AddLargeTimer(const char* label, std::shared_ptr<CPUTimer>& timer)
{
	m_Timers.emplace_back(timer);
	m_Labels.emplace_back(label);
	m_GraphSizeX.emplace_back(g_LargeGraphSizeX);
	m_GraphSizeY.emplace_back(g_LargeGraphSizeY);
}

void PlotTimersPass::AddLargeTimer(const char* label, std::shared_ptr<GLTimer>& timer)
{
	m_Timers.emplace_back(timer);
	m_Labels.emplace_back(label);
	m_GraphSizeX.emplace_back(g_LargeGraphSizeX);
	m_GraphSizeY.emplace_back(g_LargeGraphSizeY);
}


void PlotTimersPass::AddSmallTimer(const char* label, std::shared_ptr<Timer>& timer)
{
	m_Timers.emplace_back(timer);
	m_Labels.emplace_back(label);
	m_GraphSizeX.emplace_back(g_SmallGraphSizeX);
	m_GraphSizeY.emplace_back(g_SmallGraphSizeY);
}

void PlotTimersPass::AddSmallTimer(const char* label, std::shared_ptr<CPUTimer>& timer)
{
	m_Timers.emplace_back(timer);
	m_Labels.emplace_back(label);
	m_GraphSizeX.emplace_back(g_SmallGraphSizeX);
	m_GraphSizeY.emplace_back(g_SmallGraphSizeY);
}

void PlotTimersPass::AddSmallTimer(const char* label, std::shared_ptr<GLTimer>& timer)
{
	m_Timers.emplace_back(timer);
	m_Labels.emplace_back(label);
	m_GraphSizeX.emplace_back(g_SmallGraphSizeX);
	m_GraphSizeY.emplace_back(g_SmallGraphSizeY);
}


void PlotTimersPass::Render(std::vector<Renderable*>&)
{
	ImGui::Begin("Performance");

	int numTimers = int(m_Timers.size());
	for (int i = 0; i < numTimers; i++)
	{
		m_Timers[i]->GetTime();
		PlotCyclicData(m_Labels[i], (float*)m_Timers[i]->GetData(), g_NumGraphSamples, m_Timers[i]->GetOffset(), m_GraphSizeX[i], m_GraphSizeY[i]);
	}

	ImGui::End();
}


void PlotTimersPass::PlotCyclicData(const char* label, const float* data, unsigned int dataLength, unsigned int offset, int graphSizeX, int graphSizeY)
{
	ImGui::Text(label);
	std::stringstream ss;
	ss << data[(offset - 1) % dataLength] << " ms";
	ImGui::PlotLines(ss.str().c_str(), [](void* data, int idx) { return ((float*)data)[idx]; }, (float*)data, dataLength, offset, NULL, 0, FLT_MAX, ImVec2(graphSizeX, graphSizeY));
}