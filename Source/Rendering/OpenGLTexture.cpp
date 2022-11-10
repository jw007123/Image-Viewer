#include "Rendering/OpenGLTexture.h"

namespace Rendering
{
	OpenGLTexture::OpenGLTexture()
	{
		glTextIdx = GL_INVALID_INDEX;
	}


	OpenGLTexture::~OpenGLTexture()
	{
		if (glTextIdx != GL_INVALID_INDEX)
		{
			glDeleteTextures(1, &glTextIdx);
		}
	}


	bool OpenGLTexture::IsValid() const
	{
		return glTextIdx != GL_INVALID_INDEX;
	}


	void OpenGLTexture::Update(const u8* data_, const usize width_, const usize height_)
	{
		// Do we need to do anything?
		if (glTextIdx == GL_INVALID_INDEX)
		{
			glGenTextures(1, &glTextIdx);
		}
		glBindTexture(GL_TEXTURE_2D, glTextIdx);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Support non-po2 textures
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_);
		glGenerateMipmap(GL_TEXTURE_2D);

		width  = width_;
		height = height_;
	}


	GLuint OpenGLTexture::GetTextureIdx() const
	{
		return glTextIdx;
	}


	usize OpenGLTexture::GetWidth() const
	{
		return width;
	}


	usize OpenGLTexture::GetHeight() const
	{
		return height;
	}
}
