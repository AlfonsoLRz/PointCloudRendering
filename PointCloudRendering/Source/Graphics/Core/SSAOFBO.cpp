#include "stdafx.h"
#include "SSAOFBO.h"

#include "Graphics/Application/Renderer.h"
#include "Interface/Window.h"
#include "Utilities/FileManagement.h"

/// Public methods

SSAOFBO::SSAOFBO() : FBO(DEFAULT_SIZE, DEFAULT_SIZE)			// Size is updated every frame
{
	_rendParams = Renderer::getInstance()->getRenderingParameters();
	_size = Window::getInstance()->getSize();

	// Multisampled FBO ----------------------
	glGenFramebuffers(1, &_multisampledFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _multisampledFBO);

	glGenRenderbuffers(1, &_mColorBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, _mColorBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 0, GL_RGBA8, _size.x, _size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenRenderbuffers(1, &_mDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, _mDepthBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 0, GL_DEPTH_COMPONENT, _size.x, _size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _mColorBuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _mDepthBuffer);

	_success &= this->checkFBOstate();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// G-buffer ------------------- [0]
	glGenFramebuffers(1, &_gBufferFBO[0]);
	glBindFramebuffer(GL_FRAMEBUFFER, _gBufferFBO[0]);

	glGenTextures(1, &_colorBuffer[0]);
	glBindTexture(GL_TEXTURE_2D, _colorBuffer[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _size.x, _size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorBuffer[0], 0);

	// G-buffer ------------------- [1]
	glGenFramebuffers(1, &_gBufferFBO[1]);
	glBindFramebuffer(GL_FRAMEBUFFER, _gBufferFBO[1]);

	glGenTextures(1, &_colorBuffer[1]);
	glBindTexture(GL_TEXTURE_2D, _colorBuffer[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _size.x, _size.y, 0, GL_RGBA, GL_FLOAT, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorBuffer[1], 0);

	// Depth buffer
	glGenRenderbuffers(1, &_depthTexture[0]);
	glBindRenderbuffer(GL_RENDERBUFFER, _depthTexture[0]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _size.x, _size.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthTexture[0]);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	_success &= this->checkFBOstate();

	// G-buffer ------------------- [2]
	glGenFramebuffers(1, &_gBufferFBO[2]);
	glBindFramebuffer(GL_FRAMEBUFFER, _gBufferFBO[2]);

	glGenTextures(1, &_colorBuffer[2]);
	glBindTexture(GL_TEXTURE_2D, _colorBuffer[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _size.x, _size.y, 0, GL_RGBA, GL_FLOAT, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorBuffer[2], 0);

	// Depth buffer
	glGenRenderbuffers(1, &_depthTexture[1]);
	glBindRenderbuffer(GL_RENDERBUFFER, _depthTexture[1]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _size.x, _size.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthTexture[1]);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	
	_success &= this->checkFBOstate();

	// SSAO framebuffer ------------------------
	glGenFramebuffers(1, &_ssaoFramebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, _ssaoFramebufferID);

	glGenTextures(1, &_ssaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, _ssaoColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _size.x, _size.y, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ssaoColorBuffer, 0);

	unsigned int attachmentsSSAO = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &attachmentsSSAO);

	_success = this->checkFBOstate();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

SSAOFBO::~SSAOFBO()
{
	glDeleteTextures(3, _colorBuffer);
	//glDeleteRenderbuffers(1, &_depthTexture);
}

bool SSAOFBO::bindFBO()
{
	if (!_success)
	{
		return false;
	}

	return true;
}

bool SSAOFBO::bindGBufferFBO(const unsigned index)
{
	if (!_success)
	{
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, _gBufferFBO[index]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return true;
}

bool SSAOFBO::bindMultisamplingFBO()
{
	if (!_success)
	{
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, _multisampledFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return true;
}

bool SSAOFBO::bindSSAOFBO()
{
	if (!_success)
	{
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, _ssaoFramebufferID);
	glClear(GL_COLOR_BUFFER_BIT);

	return true;
}

void SSAOFBO::bindGBufferTextures(RenderingShader* shader)
{
	shader->setUniform("texPosition", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _colorBuffer[1]);

	shader->setUniform("texNormal", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _colorBuffer[2]);
}

void SSAOFBO::bindSSAOTexture(RenderingShader* shader)
{
	shader->setUniform("texAlbedo", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _colorBuffer[0]);

	shader->setUniform("texSSAO", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _ssaoColorBuffer);
}

void SSAOFBO::modifySize(const uint16_t width, const uint16_t height)
{
	FBO::modifySize(width, height);

	// Multisampling buffer
	glBindFramebuffer(GL_FRAMEBUFFER, _multisampledFBO);

	glBindRenderbuffer(GL_RENDERBUFFER, _mColorBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 0, GL_RGBA8, _size.x, _size.y);

	glBindRenderbuffer(GL_RENDERBUFFER, _mDepthBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 0, GL_DEPTH_COMPONENT, _size.x, _size.y);

	// GBuffer
	glBindFramebuffer(GL_FRAMEBUFFER, _id);

	for (unsigned colorBuffer = 0; colorBuffer < 3; colorBuffer++)
	{
		glBindTexture(GL_TEXTURE_2D, _colorBuffer[colorBuffer]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _size.x, _size.y, 0, GL_RGBA, colorBuffer ? GL_FLOAT : GL_UNSIGNED_BYTE, nullptr);			// Size is already updated
	}

	for (unsigned depthBuffer = 0; depthBuffer < 2; depthBuffer++)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, _depthTexture[depthBuffer]);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _size.x, _size.y);
	}

	// SSAO buffer
	glBindFramebuffer(GL_FRAMEBUFFER, _ssaoFramebufferID);

	glBindTexture(GL_TEXTURE_2D, _ssaoColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _size.x, _size.y, 0, GL_RGBA, GL_FLOAT, nullptr);		// Size is already updated

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void SSAOFBO::writeGBuffer(const unsigned index)
{
	if (!_success)
	{
		return;
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, _multisampledFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _gBufferFBO[index]);
	glBlitFramebuffer(0, 0, _size.x, _size.y, 0, 0, _size.x, _size.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	//glBindFramebuffer(GL_FRAMEBUFFER, _gBufferFBO[index]);

	//std::vector<GLubyte>* pixels = new std::vector<GLubyte>((int)_size.x * (int)_size.y * 4);
	//glReadPixels(0, 0, _size.x, _size.y, GL_RGBA, GL_UNSIGNED_BYTE, pixels->data());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//// Correct image orientation
	//Image::flipImageVertically(*pixels, _size.x, _size.y, 4);

	//// Launch image writing in a thread
	//std::thread writeImageThread(&SSAOFBO::threadedWriteImage, this, pixels, "Prueba.png", _size.x, _size.y);
	//writeImageThread.detach();
}

/// Protected methods

void SSAOFBO::threadedWriteImage(std::vector<GLubyte>* pixels, const std::string& filename, const uint16_t width, const uint16_t height)
{
	FileManagement::saveImage(filename, pixels, width, height);

	delete pixels;
}