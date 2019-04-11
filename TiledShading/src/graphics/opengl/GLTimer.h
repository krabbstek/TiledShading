#pragma once

#include <glad/glad.h>

#include "Timer.h"

class GLTimer : public Timer
{
public:
	GLTimer();
	~GLTimer();

	void Start() override;
	void Stop() override;
	void GetTime() override;

private:
	GLuint m_QueryID[2];
};