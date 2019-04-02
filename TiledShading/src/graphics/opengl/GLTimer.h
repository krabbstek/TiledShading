#pragma once

#include <glad/glad.h>

class GLTimer
{
public:
	GLTimer();
	~GLTimer();

	void Start();
	void Stop();
	unsigned long long GetTime() const;

private:
	GLuint m_QueryID[2];
};