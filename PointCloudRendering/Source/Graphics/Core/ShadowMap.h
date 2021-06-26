#pragma once

#include "Graphics/Core/FBO.h"
#include "Graphics/Core/ShaderProgram.h"

/**
*	@file ShadowMap.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/17/2019
*/

/**
*	@brief FBO which contains a depth buffer with the scene seen from a light source.
*/
class ShadowMap: public FBO
{
protected:
	GLuint _depthBuffer;						//!< Identifier of depth buffer assigned to FBO

public:
	/**
	*	@brief Constructor.
	*	@param width Width of depth buffer.
	*	@param height Height of depth buffer.
	*/
	ShadowMap(const uint16_t width, const uint16_t height);

	/**
	*	@brief Destructor.
	*/
	virtual ~ShadowMap();

	/**
	*	@brief Binds the FBO so any rendering can get captured here.
	*/
	virtual bool bindFBO();

	/**
	*	@brief Sets the depth buffer as an uniform for a shader (sampler).
	*	@param shader Shader where we should specify the depth buffer as a texture.
	*/
	void applyTexture(ShaderProgram* shader);

	/**
	*	@brief Modifies the size specified in textures related to framebuffer.
	*	@param width New width.
	*	@param height New height.
	*/
	virtual void modifySize(const uint16_t width, const uint16_t height);
};

