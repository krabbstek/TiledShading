#pragma once

#include <glad/glad.h>

#include "ConstExpr.h"

class GLTimer
{
public:
	GLTimer();
	~GLTimer();

	void Start();
	void Stop();
	void GetTime();

	inline const float* GetData() const { return m_Data; }
	inline unsigned int GetOffset() const { return m_Offset; }

private:
	GLuint m_QueryID[2];
	float m_Data[g_NumGraphSamples];
	int m_Offset;
};