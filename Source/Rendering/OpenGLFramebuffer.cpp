#include "Rendering/OpenGLFramebuffer.h"

namespace Rendering
{
	OpenGLFramebuffer::OpenGLFramebuffer(const usize width_, const usize height_)
	{
		glGenFramebuffers(1, &glMSAAFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, glMSAAFBO);

		glGenTextures(1, &glMSAAText);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, glMSAAText);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGB, width_, height_, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, glMSAAText, 0);

		glGenRenderbuffers(1, &glRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, glRBO);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH24_STENCIL8, width_, height_);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, glRBO);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			assert(0);
		}

		glGenFramebuffers(1, &glFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, glFBO);

		glGenTextures(1, &glFBOText);
		glBindTexture(GL_TEXTURE_2D, glFBOText);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, glFBOText, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			assert(0);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		width  = width_;
		height = height_;
	}


	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &glMSAAFBO);
		glDeleteFramebuffers(1, &glFBO);
		glDeleteTextures(1, &glMSAAText);
		glDeleteTextures(1, &glFBOText);
		glDeleteRenderbuffers(1, &glRBO);
	}


	void OpenGLFramebuffer::Resize(const usize width_, const usize height_)
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, glMSAAText);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGB, width_, height_, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, glRBO);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH24_STENCIL8, width_, height_);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glBindTexture(GL_TEXTURE_2D, glFBOText);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		width  = width_;
		height = height_;
	}


	void OpenGLFramebuffer::Use()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, glMSAAFBO);

		glViewport(0, 0, width, height);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
	}


	void OpenGLFramebuffer::Release()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	void OpenGLFramebuffer::RenderToTexture()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, glMSAAFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, glFBO);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		ImGui::Image((GLuint*)glFBOText, ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
	}
}
