#include "GLImageTexture2D.h"

#include "GLCommon.h"

#include <iostream>


GLImageTexture2D::GLImageTexture2D(GLuint storageFormat, GLuint access, unsigned int width, unsigned int height)
	: m_StorageFormat(storageFormat), m_Access(access), m_Width(width), m_Height(height)
{
	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
	GLCall(glTexStorage2D(GL_TEXTURE_2D, 1, storageFormat, m_Width, m_Height));
}

GLImageTexture2D::~GLImageTexture2D()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}


void GLImageTexture2D::SetWrapS(GLuint parameter) const
{
	Bind();
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, parameter));
}

void GLImageTexture2D::SetWrapT(GLuint parameter) const
{
	Bind();
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, parameter));
}

void GLImageTexture2D::SetWrapST(GLuint parameter) const
{
	Bind();
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, parameter));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, parameter));
}

void GLImageTexture2D::SetMinFilter(GLuint parameter) const
{
	Bind();
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, parameter));
}

void GLImageTexture2D::SetMagFilter(GLuint parameter) const
{
	Bind();
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, parameter));
}

void GLImageTexture2D::SetMinMagFilter(GLuint parameter) const
{
	Bind();
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, parameter));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, parameter));
}


void GLImageTexture2D::Bind(unsigned int slot) const
{
	GLCall(glBindImageTexture(slot, m_RendererID, 0, GL_FALSE, 0, m_Access, m_StorageFormat));
}

void GLImageTexture2D::BindAsTexture(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void GLImageTexture2D::Unbind(unsigned int slot)
{
	GLCall(glBindImageTexture(slot, 0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32I));
}