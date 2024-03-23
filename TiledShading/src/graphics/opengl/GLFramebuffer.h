#pragma once

#include "GLTexture2D.h"

#include <glad/glad.h>

class GLFramebuffer
{
public:
	GLFramebuffer(unsigned int width, unsigned int height);
	~GLFramebuffer();

	/* type = GL_RGB or similar. attachment = GL_COLOR_ATTACHMENT(0,1,2) or GL_DEPTH_STENCIL_ATTACHMENT. */
	GLTexture2D* AttachTexture(GLuint internalFormat, GLuint storageType, unsigned int attachment = 0) const;
	/* type = GL_RGB or similar. attachment = GL_COLOR_ATTACHMENT(0,1,2) or GL_DEPTH_STENCIL_ATTACHMENT. */
	void AttachTexture(GLTexture2D& texture, GLuint internalFormat, GLuint storageType, unsigned int attachment = 0) const;

	void GenerateDepthStencilRenderbuffer();
		
	void SetDrawBufferAttachments(unsigned int count) const;
	void SetDrawBufferAttachments(const GLenum* attachments, unsigned int count) const;

	void ClearColor(float r, float g, float b, float a) const;
	static void Clear(GLuint mask);

	void EnableBlending(GLenum srcFactor, GLenum dstFactor) const;
	static void GLFramebuffer::EnableDefaultBlending(GLenum srcFactor, GLenum dstFactor);

	void Bind() const;
	static void Unbind();
	inline static void SetDefaultFramebuffer() { Unbind(); }

	static GLuint GetBaseFormat(GLuint internalFormat);

private:
	unsigned int m_Width, m_Height;
	unsigned int m_NumAttachedTextures;
	GLuint m_FramebufferRendererID;
	GLuint m_DepthStencilRenderbufferID;
};
