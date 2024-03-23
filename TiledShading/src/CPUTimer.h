#pragma once

#include "Timer.h"

#include <chrono>

class CPUTimer : public Timer
{
public:
	CPUTimer();
	~CPUTimer();

	void Start() override;
	void Stop() override;
	void GetTime() override;

private:
	std::chrono::high_resolution_clock::time_point m_Start;
	std::chrono::high_resolution_clock::time_point m_Stop;
};