#pragma once

#include <vector>
#include <glad/glad.h>

typedef std::pair<GLuint, unsigned int> LayoutElement;

class GLVertexBufferLayout
{
public:
	GLVertexBufferLayout();

	void Push(GLuint type, unsigned int count);

	inline const std::vector<LayoutElement>& GetLayoutElements() const { return m_Layout; }
	inline unsigned int GetStride() const { return m_Stride; }

	static unsigned int GetElementSize(GLuint type);

private:
	std::vector<LayoutElement> m_Layout;
	unsigned int m_Stride;
};
