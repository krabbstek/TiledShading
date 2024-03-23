#pragma once

#include "ConstExpr.h"

#include <memory>

class Timer
{
public:
	Timer() 
		: m_Offset(0) { memset(m_Data, 0, sizeof(m_Data)); }

	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual void GetTime() = 0;

	inline const float* GetData() const { return m_Data; }
	inline unsigned int GetOffset() const { return m_Offset; }

protected:
	float m_Data[g_NumGraphSamples];
	int m_Offset;
};