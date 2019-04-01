#pragma once

#include <glad/glad.h>

#define MAX_TEXTURES 16

class GLImageTexture2D
{
public:
	GLImageTexture2D(GLuint storageFormat, GLuint access, unsigned int width, unsigned int height);
	~GLImageTexture2D();

	void SetWrapS(GLuint parameter) const;
	void SetWrapT(GLuint parameter) const;
	void SetWrapST(GLuint parameter) const;
	void SetMinFilter(GLuint parameter) const;
	void SetMagFilter(GLuint parameter) const;
	void SetMinMagFilter(GLuint parameter) const;

	void Bind(unsigned int slot = 0) const;
	static void Unbind(unsigned int slot = 0);

	inline unsigned int GetWidth() const { return m_Width; }
	inline unsigned int GetHeight() const { return m_Height; }
	inline GLuint RendererID() const { return m_RendererID; }

private:
	GLuint m_RendererID;
	GLuint m_StorageFormat;
	GLuint m_Access;
	unsigned int m_Width, m_Height;
};