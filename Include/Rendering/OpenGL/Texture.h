#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "Eigen/Core"

#include "Utility/Literals.h"

namespace Rendering
{
	class Texture
	{
	public:
		enum TextureFilter : u8
		{
			Nearest  = 0,
			Bilinear = 1,
			Num
		};

		Texture();
		~Texture();

		/// data_ assumed to be RGBA
		void Update(const TextureFilter& type_, const u8* data_, const usize width_, const usize height_);

		bool IsValid() const;

		GLuint GetTextureIdx() const;
		usize  GetWidth() const;
		usize  GetHeight() const;

	private:
		GLuint glTextIdx;

		usize width;
		usize height;
	};
}
