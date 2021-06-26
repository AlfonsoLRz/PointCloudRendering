#include "stdafx.h"
#include "ShadowMap.h"

#include "Graphics/Application/Renderer.h"

/// [Public methods]

ShadowMap::ShadowMap(const uint16_t width, const uint16_t height):
	FBO(width, height)
{
	GLfloat border[] = { 1.0f, 1.0f, 1.0f, 1.0f };						// Color out of boundaries [0, 1]. 1 is the maximum depth, so no fragment can't be in shadow because of this

	glGenFramebuffers(1, &_id);
	glBindFramebuffer(GL_FRAMEBUFFER, _id);

	// Depth buffer
	glGenTextures(1, &_depthBuffer);
	glBindTexture(GL_TEXTURE_2D, _depthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, _size.x, _size.y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthBuffer, 0);				// Assign depth buffer to FBO

	this->checkFBOstate();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowMap::~ShadowMap()
{
	glDeleteTextures(1, &_depthBuffer);
}

bool ShadowMap::bindFBO()
{
	if (!_success)
	{
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, _id);
	glActiveTexture(GL_TEXTURE0 + Texture::SHADOW_MAP_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, _depthBuffer);

	return true;
}

void ShadowMap::applyTexture(ShaderProgram* shader)
{
	const int textureID = Texture::SHADOW_MAP_TEXTURE;

	shader->setUniform("texShadowMapSampler", textureID);
	glActiveTexture(GL_TEXTURE0 + textureID);
	glBindTexture(GL_TEXTURE_2D, _depthBuffer);
}

void ShadowMap::modifySize(const uint16_t width, const uint16_t height)
{
	FBO::modifySize(width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, _id);
	glBindTexture(GL_TEXTURE_2D, _depthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, _size.x, _size.y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
