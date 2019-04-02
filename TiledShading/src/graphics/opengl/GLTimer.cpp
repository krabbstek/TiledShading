#include "GLTimer.h"

#include "GLCommon.h"

GLTimer::GLTimer()
{
	GLCall(glGenQueries(2, m_QueryID));
	Start();
	Stop();
}

GLTimer::~GLTimer()
{
	GLCall(glDeleteQueries(2, m_QueryID));
}


void GLTimer::Start()
{
	GLCall(glQueryCounter(m_QueryID[0], GL_TIMESTAMP));
}

void GLTimer::Stop()
{
	GLCall(glQueryCounter(m_QueryID[1], GL_TIMESTAMP));
}

unsigned long long GLTimer::GetTime() const
{
	GLuint64 startTime, endTime = 0;
	while (!endTime)
	{
		GLCall(glGetQueryObjectui64v(m_QueryID[1], GL_QUERY_RESULT_AVAILABLE, &endTime));
	}

	GLCall(glGetQueryObjectui64v(m_QueryID[0], GL_QUERY_RESULT, &startTime));
	GLCall(glGetQueryObjectui64v(m_QueryID[1], GL_QUERY_RESULT, &endTime));

	return endTime - startTime;
}