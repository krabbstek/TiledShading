#include "CPUTimer.h"

CPUTimer::CPUTimer()
	: Timer()
{
	Start();
	Stop();
}

CPUTimer::~CPUTimer()
{
}


void CPUTimer::Start()
{
	m_Start = std::chrono::high_resolution_clock::now();
}

void CPUTimer::Stop()
{
	m_Stop = std::chrono::high_resolution_clock::now();
}

void CPUTimer::GetTime()
{
	std::chrono::duration<float, std::milli> dt = m_Stop - m_Start;

	m_Data[m_Offset] = dt.count();
	m_Offset = (m_Offset + 1) % g_NumGraphSamples;
}