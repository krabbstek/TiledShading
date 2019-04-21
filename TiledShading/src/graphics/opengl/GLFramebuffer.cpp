#include "GLFramebuffer.h"

#include "GLCommon.h"

#include <iostream>

GLFramebuffer::GLFramebuffer(unsigned int width, unsigned int height)
	: m_Width(width), m_Height(height)
{
	GLCall(glGenFramebuffers(1, &m_FramebufferRendererID));

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		
		("Failed to generate framebuffer!");
		__debugbreak();
	}
}

GLFramebuffer::~GLFramebuffer()
{
	if (m_FramebufferRendererID)
	{
		GLCall(glDeleteFramebuffers(1, &m_FramebufferRendererID));
	}
	if (m_DepthStencilRenderbufferID)
	{
		GLCall(glDeleteRenderbuffers(1, &m_DepthStencilRenderbufferID));
	}
}


GLFramebuffer* GLFramebuffer::GetDefaultFramebuffer()
{
	return new GLFramebuffer();
}


GLTexture2D* GLFramebuffer::AttachTexture(GLuint internalFormat, GLuint storageType, unsigned int attachment /*= 0*/) const
{
	GLint fbDim[4];
	GLCall(glGetIntegerv(GL_VIEWPORT, fbDim));

	GLuint format = GetBaseFormat(internalFormat);
	if (!format)
	{
		std::printf("Invalid format!");
		__debugbreak();
	}

	GLTexture2D* texture = new GLTexture2D();
	texture->Load(internalFormat, NULL, /*width =*/ fbDim[2], /*height =*/ fbDim[3], format, storageType);
	texture->SetMinMagFilter(GL_LINEAR);
	texture->SetWrapST(GL_CLAMP_TO_EDGE);

	Bind();
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, GL_TEXTURE_2D, texture->m_RendererID, 0));

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::printf("Failed to attach color buffer!");
		__debugbreak();
	}

	return texture;
}

void GLFramebuffer::AttachTexture(GLTexture2D& texture, GLuint internalFormat, GLuint storageType, unsigned int attachment /*= 0*/) const
{
	GLuint format = GetBaseFormat(internalFormat);
	texture.Load(internalFormat, NULL, /*width =*/ m_Width, /*height =*/ m_Height, format, storageType);
	texture.SetMinMagFilter(GL_NEAREST);
	texture.SetWrapST(GL_CLAMP_TO_EDGE);

	Bind();
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, GL_TEXTURE_2D, texture.m_RendererID, 0));

	GLenum error;
	GLCall(error = glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if (error != GL_FRAMEBUFFER_COMPLETE)
	{
		std::printf("Failed to attach color buffer!");
		__debugbreak();
	}
}


void GLFramebuffer::GenerateDepthStencilRenderbuffer()
{
	if (m_DepthStencilRenderbufferID)
	{
		GLCall(glDeleteRenderbuffers(1, &m_DepthStencilRenderbufferID));
	}

	GLint fbDim[4];
	GLCall(glGetIntegerv(GL_VIEWPORT, fbDim));

	GLCall(glGenRenderbuffers(1, &m_DepthStencilRenderbufferID));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_DepthStencilRenderbufferID));
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, /*width =*/ fbDim[2], /*height =*/ fbDim[3]));

	Bind();
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthStencilRenderbufferID);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::printf("Failed to generate depth stencil buffer!");
		__debugbreak();
	}
}


void GLFramebuffer::SetDrawBufferAttachments(unsigned int count) const
{
	GLenum* attachments = new GLenum[count];
	for (unsigned int i = 0; i < count; i++)
		attachments[i] = GL_COLOR_ATTACHMENT0 + i;
	SetDrawBufferAttachments(attachments, count);
	delete[] attachments;
}

void GLFramebuffer::SetDrawBufferAttachments(const GLenum* attachments, unsigned int count) const
{
	GLenum error;
	GLCall(glDrawBuffers(count, attachments));
	GLCall(error = glCheckNamedFramebufferStatus(m_FramebufferRendererID, GL_FRAMEBUFFER));
	if (error != GL_FRAMEBUFFER_COMPLETE)
	{
		std::printf("Failed to set drawbuffer attachments");
		__debugbreak();
	}
}


void GLFramebuffer::ClearColor(float r, float g, float b, float a) const
{
	Bind();
	GLCall(glClearColor(r, g, b, a));
}

void GLFramebuffer::Clear(GLuint mask)
{
	GLCall(glClear(mask));
}


void GLFramebuffer::EnableBlending(GLenum srcFactor, GLenum dstFactor) const
{
	Bind();
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(srcFactor, dstFactor));
}

void GLFramebuffer::EnableDefaultBlending(GLenum srcFactor, GLenum dstFactor)
{
	SetDefaultFramebuffer();
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(srcFactor, dstFactor));
}


void GLFramebuffer::Bind() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferRendererID));
	GLCall(glViewport(0, 0, m_Width, m_Height));
}

void GLFramebuffer::Unbind()
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}


GLuint GLFramebuffer::GetBaseFormat(GLuint internalFormat)
{
	switch (internalFormat)
	{
	case GL_R8:
	case GL_R8_SNORM:
	case GL_R16:
	case GL_R16_SNORM:
	case GL_R16F:
	case GL_R32F:
	case GL_R8I:
	case GL_R8UI:
	case GL_R16I:
	case GL_R16UI:
	case GL_R32I:
	case GL_R32UI:
		return GL_RED;
				 
	case GL_RG8:
	case GL_RG8_SNORM:
	case GL_RG16:
	case GL_RG16_SNORM:
	case GL_RG16F:
	case GL_RG32F:
	case GL_RG8I:
	case GL_RG8UI:
	case GL_RG16I:
	case GL_RG16UI:
	case GL_RG32I:
	case GL_RG32UI:
		return GL_RG;

	case GL_R3_G3_B2:
	case GL_RGB4:
	case GL_RGB5:
	case GL_RGB8:
	case GL_RGB8_SNORM:
	case GL_RGB10:
	case GL_RGB12:
	case GL_RGB16_SNORM:
	case GL_RGBA2:
	case GL_RGBA4:
	case GL_SRGB8:
	case GL_RGB16F:
	case GL_RGB32F:
	case GL_R11F_G11F_B10F:
	case GL_RGB9_E5:
	case GL_RGB8I:
	case GL_RGB8UI:
	case GL_RGB16I:
	case GL_RGB16UI:
	case GL_RGB32I:
	case GL_RGB32UI:
		return GL_RGB;

	case GL_RGB5_A1:
	case GL_RGBA8:
	case GL_RGBA8_SNORM:
	case GL_RGB10_A2:
	case GL_RGB10_A2UI:
	case GL_RGBA12:
	case GL_RGBA16:
	case GL_SRGB8_ALPHA8:
	case GL_RGBA16F:
	case GL_RGBA32F:
	case GL_RGBA8I:
	case GL_RGBA8UI:
	case GL_RGBA16I:
	case GL_RGBA16UI:
	case GL_RGBA32I:
	case GL_RGBA32UI:
		return GL_RGBA;

	default:
		return internalFormat;
	}
}


GLFramebuffer::GLFramebuffer()
	: m_NumAttachedTextures(0), m_FramebufferRendererID(0), m_DepthStencilRenderbufferID(0)
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	int v[4];
	GLCall(glGetIntegerv(GL_VIEWPORT, v));

	m_Width = v[2];
	m_Height = v[3];
}