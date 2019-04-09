#pragma once

#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "math/mat4.h"

#include <glad/glad.h>
#include <unordered_map>

class GLShader
{
public:
	GLShader();
	~GLShader();

	inline void AddShader(GLuint shaderType, const char* shaderSource)
	{
		m_Shaders[shaderType] = shaderSource;
	}
	void AddShaderFromFile(GLuint shaderType, const char* filePath);
	bool CompileShaders();

	void Bind() const;
	void Unbind() const;

	void DispatchComputeShader(unsigned int numWorkGroupsX, unsigned int numWorkGroupsY, unsigned int numWorkGroupsZ);

	inline void SetUniform1f(const char* uniformName, float value) { SetUniform1f(std::string(uniformName), value); }
	void SetUniform1f(const std::string& uniformName, float value);
	inline void SetUniform1i(const char* uniformName, int value) { SetUniform1i(std::string(uniformName), value); }
	void SetUniform1i(const std::string& uniformName, int value);

	inline void SetUniform1fv(const char* uniformName, float* values, unsigned int count) { SetUniform1fv(std::string(uniformName), values, count); }
	void SetUniform1fv(const std::string& uniformName, float* values, unsigned int count);

	inline void SetUniform2f(const char* uniformName, float x, float y) { return SetUniform2f(std::string(uniformName), x, y); }
	void SetUniform2f(std::string& uniformName, float x, float y);
	inline void SetUniform2f(const char* uniformName, const vec2& vec) { return SetUniform2f(std::string(uniformName), vec); }
	void SetUniform2f(std::string& uniformName, const vec2& vec);

	inline void SetUniform3f(const char* uniformName, float x, float y, float z) { return SetUniform3f(std::string(uniformName), x, y, z); }
	void SetUniform3f(std::string& uniformName, float x, float y, float z);
	inline void SetUniform3f(const char* uniformName, const vec3& vec) { return SetUniform3f(std::string(uniformName), vec); }
	void SetUniform3f(std::string& uniformName, const vec3& vec);

	inline void SetUniform4f(const char* uniformName, float x, float y, float z, float w) { return SetUniform4f(std::string(uniformName), x, y, z, w); }
	void SetUniform4f(std::string& uniformName, float x, float y, float z, float w);
	inline void SetUniform4f(const char* uniformName, const vec4& vec) { return SetUniform4f(std::string(uniformName), vec); }
	void SetUniform4f(std::string& uniformName, const vec4& vec);

	inline void SetUniformMat4(const char* uniformName, const mat4& matrix) { SetUniformMat4(std::string(uniformName), matrix); }
	void SetUniformMat4(const std::string& uniformName, const mat4& matrix);

	unsigned int GetUniformLocation(const std::string& uniformName);

private:
	GLuint m_RendererID;
	std::unordered_map<GLuint, const char*> m_Shaders;
	std::vector<char*> m_TempSource;
	std::unordered_map<std::string, unsigned int> m_UniformLocations;
};