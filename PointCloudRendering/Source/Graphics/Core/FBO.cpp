#include "stdafx.h"
#include "FBO.h"

#include "Graphics/Core/OpenGLUtilities.h"

/// [Public methods]

FBO::FBO(const uint16_t width, const uint16_t height):
	_id(0), _size(width, height), _success(true)
{
}

FBO::~FBO()
{
	glDeleteFramebuffers(1, &_id);
}

void FBO::modifySize(const uint16_t width, const uint16_t height)
{
	_size = vec2(width, height);
}

/// [Protected methods]

bool FBO::checkFBOstate()
{
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "FBO could not be created" << std::endl;

		return false;
	}

	return true;
}