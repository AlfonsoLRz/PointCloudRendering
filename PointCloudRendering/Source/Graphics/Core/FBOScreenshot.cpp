#include "stdafx.h"
#include "FBOScreenshot.h"

#include "Graphics/Application/Renderer.h"
#include "Utilities/FileManagement.h"

/// [Public methods]

FBOScreenshot::FBOScreenshot(const uint16_t width, const uint16_t height):
	FBO(width, height)
{
	glGenFramebuffers(1, &_id);									// Support for multisampled framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, _id);

	glGenTextures(1, &_colorBufferID);
	glBindTexture(GL_TEXTURE_2D, _colorBufferID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _size.x, _size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Color buffer => FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorBufferID, 0);

	_success = this->checkFBOstate();

	// Multisampled FBO
	glGenFramebuffers(1, &_multisampledFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _multisampledFBO);

	// New color buffer for multisampled FBO
	glGenRenderbuffers(1, &_mColorBufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, _mColorBufferID);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 0, GL_RGBA8, _size.x, _size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Depth buffer for multisampled FBO
	glGenRenderbuffers(1, &_mDepthBufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, _mDepthBufferID);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 0, GL_DEPTH_COMPONENT, _size.x, _size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Color & depth buffer => Multisampled FBO
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _mColorBufferID);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _mDepthBufferID);

	_success &= this->checkFBOstate();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);				// Default framebuffer
}

FBOScreenshot::~FBOScreenshot()
{
	glDeleteTextures(1, &_colorBufferID);
	glDeleteRenderbuffers(1, &_mColorBufferID);
	glDeleteRenderbuffers(1, &_mDepthBufferID);
}

bool FBOScreenshot::bindFBO()
{
	if (!_success)
	{
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, _multisampledFBO);			// Multisampled! Normal FBO is only used as support for this one

	return true;
}

Image* FBOScreenshot::getImage() const
{
	if (!_success)
	{
		return nullptr;
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, _multisampledFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id);
	glBlitFramebuffer(0, 0, _size.x, _size.y, 0, 0, _size.x, _size.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glBindFramebuffer(GL_FRAMEBUFFER, _id);

	GLubyte* pixels = (GLubyte*)malloc((int) _size.x * (int) _size.y * 4);
	glReadPixels(0, 0, _size.x, _size.y, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Image* image = new Image(pixels, _size.x, _size.y, 4);
	free(pixels);

	return image;
}

void FBOScreenshot::modifySize(const uint16_t width, const uint16_t height)
{
	FBO::modifySize(width, height);

	if (_success)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _id);

		glBindTexture(GL_TEXTURE_2D, _colorBufferID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _size.x, _size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, _multisampledFBO);

		glBindRenderbuffer(GL_RENDERBUFFER, _mColorBufferID);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 0, GL_RGBA8, _size.x, _size.y);

		glBindRenderbuffer(GL_RENDERBUFFER, _mDepthBufferID);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 0, GL_DEPTH_COMPONENT, _size.x, _size.y);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

bool FBOScreenshot::saveImage(const std::string& filename)
{
	Image* image = this->getImage();

	if (image)
	{
		image->flipImageVertically();
		image->saveImage(filename);

		return true;
	}

	return false;
}

