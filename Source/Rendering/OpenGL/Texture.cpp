#include "Rendering/OpenGL/Texture.h"

namespace Rendering
{
	Texture::Texture()
	{
		glTextIdx = GL_INVALID_INDEX;
	}


	Texture::~Texture()
	{
		if (glTextIdx != GL_INVALID_INDEX)
		{
			glDeleteTextures(1, &glTextIdx);
		}
	}


	bool Texture::IsValid() const
	{
		return glTextIdx != GL_INVALID_INDEX;
	}


	void Texture::Update(const TextureFilter& type_, const u8* data_, const usize width_, const usize height_)
	{
		// Do we need to do anything?
		if (glTextIdx == GL_INVALID_INDEX)
		{
			glGenTextures(1, &glTextIdx);
		}
		glBindTexture(GL_TEXTURE_2D, glTextIdx);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (type_ == TextureFilter::Nearest)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else if (type_ == TextureFilter::Bilinear)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			assert(0);
		}
	

		// Support non-po2 textures
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_);
		glGenerateMipmap(GL_TEXTURE_2D);

		width  = width_;
		height = height_;
	}


	GLuint Texture::GetTextureIdx() const
	{
		return glTextIdx;
	}


	usize Texture::GetWidth() const
	{
		return width;
	}


	usize Texture::GetHeight() const
	{
		return height;
	}
}
