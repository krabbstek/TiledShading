#include "GLShader.h"

#include "GLCommon.h"

#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "math/mat4.h"

#include <fstream>
#include <sstream>

GLShader::GLShader()
	: m_RendererID(0)
{
}

GLShader::~GLShader()
{
	if (m_RendererID)
	{
		GLCall(glDeleteProgram(m_RendererID));
	}
}


void GLShader::AddShaderFromFile(GLuint shaderType, const char* filePath)
{
	if (filePath)
	{
		std::ifstream t("file.txt");
		std::stringstream buffer;
		buffer << t.rdbuf();

		char* c = new char[buffer.str().size() + 1];
		c[buffer.str().size()] = 0;
		memcpy(c, buffer.str().c_str(), buffer.str().size());

		m_TempSource.push_back(c);

		AddShader(shaderType, c);
	}
	else
		std::printf("Failed to read file - nullptr filePath.");
}

bool GLShader::CompileShaders()
{
	std::vector<GLuint> compiledShaders;
	GLint result = 0;
	GLint length = 0;
	constexpr GLint maxLength = 256;
	char info[maxLength];

	for (auto a : m_Shaders)
	{
		GLCall(GLuint shader = glCreateShader(a.first));
		GLCall(glShaderSource(shader, 1, &a.second, 0));
		GLCall(glCompileShader(shader));
			
		GLCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
		if (result == GL_FALSE)
		{
			GLCall(glGetShaderInfoLog(shader, maxLength, &length, info));
			GLCall(glDeleteShader(shader));
			for (GLuint sh : compiledShaders)
			{
				GLCall(glDeleteShader(sh));
			}
			std::printf("Failed to compile shader (type {}): {}", a.first, info);
			return false;
		}

		compiledShaders.push_back(shader);
	}

	GLCall(m_RendererID = glCreateProgram());
	for (GLuint shader : compiledShaders)
	{
		GLCall(glAttachShader(m_RendererID, shader));
	}
	GLCall(glLinkProgram(m_RendererID));

	GLCall(glGetProgramiv(m_RendererID, GL_LINK_STATUS, &result));
	if (result == GL_FALSE)
	{
		GLCall(glGetProgramInfoLog(m_RendererID, length, &length, &info[0]));
		GLCall(glDeleteProgram(m_RendererID));
		m_RendererID = 0;
		for (GLuint shader : compiledShaders)
		{
			GLCall(glDeleteShader(shader));
		}
		std::printf("Failed to link shaders: {}", info);
		return false;
	}

	for (GLuint shader : compiledShaders)
	{
		GLCall(glDetachShader(m_RendererID, shader));
	}
	while (m_TempSource.size())
	{
		delete m_TempSource.back();
		m_TempSource.pop_back();
	}

	return true;
}


void GLShader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void GLShader::Unbind() const
{
	GLCall(glUseProgram(0));
}


void GLShader::SetUniform1f(const std::string& uniformName, float value)
{
	int location = GetUniformLocation(uniformName);
	GLCall(glProgramUniform1f(m_RendererID, location, value));
}

void GLShader::SetUniform1i(const std::string& uniformName, int value)
{
	int location = GetUniformLocation(uniformName);
	GLCall(glProgramUniform1i(m_RendererID, location, value));
}


void GLShader::SetUniform1fv(const std::string& uniformName, float* values, unsigned int count)
{
	int location = GetUniformLocation(uniformName);
	GLCall(glProgramUniform1fv(m_RendererID, location, count, values));
}


void GLShader::SetUniform2f(std::string& uniformName, float x, float y)
{
	int location = GetUniformLocation(uniformName);
	GLCall(glProgramUniform2f(m_RendererID, location, x, y));
}

void GLShader::SetUniform2f(std::string& uniformName, const vec2& vec)
{
	int location = GetUniformLocation(uniformName);
	GLCall(glProgramUniform2fv(m_RendererID, location, 1, &vec.x));
}


void GLShader::SetUniform3f(std::string& uniformName, float x, float y, float z)
{
	int location = GetUniformLocation(uniformName);
	GLCall(glProgramUniform3f(m_RendererID, location, x, y, z));
}

void GLShader::SetUniform3f(std::string& uniformName, const vec3& vec)
{
	int location = GetUniformLocation(uniformName);
	GLCall(glProgramUniform3fv(m_RendererID, location, 1, &vec.x));
}


void GLShader::SetUniform4f(std::string& uniformName, float x, float y, float z, float w)
{
	int location = GetUniformLocation(uniformName);
	GLCall(glProgramUniform4f(m_RendererID, location, x, y, z, w));
}

void GLShader::SetUniform4f(std::string& uniformName, const vec4& vec)
{
	int location = GetUniformLocation(uniformName);
	GLCall(glProgramUniform4fv(m_RendererID, location, 1, &vec.x));
}


void GLShader::SetUniformMat4(const std::string& uniformName, const mat4& matrix)
{
	int location = GetUniformLocation(uniformName);
	GLCall(glProgramUniformMatrix4fv(m_RendererID, location, 1, GL_FALSE, &matrix.elements[0]));
}


unsigned int GLShader::GetUniformLocation(const std::string& uniformName)
{
	auto entry = m_UniformLocations.find(uniformName);
	// Uniform location not found in map
	if (entry == m_UniformLocations.end())
	{
		GLCall(unsigned int location = glGetUniformLocation(m_RendererID, uniformName.c_str()));
		m_UniformLocations[uniformName] = location;
		return location;
	}
	// Uniform location found in map
	else
		return entry->second;
}