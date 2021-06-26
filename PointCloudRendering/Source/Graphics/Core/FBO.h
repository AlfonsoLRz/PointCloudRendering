#pragma once

#include "Graphics/Core/VAO.h"

/**
*	@file FBO.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/26/2019
*/

/**
*	@brief Additional framebuffer to perform operations.
*/
class FBO
{
protected:
	GLuint	_id;							//!< Identifier given by OpenGL
	ivec2	_size;							//!< Dimensions of buffer
	bool	_success;						//!< Success of declaring framebuffers

protected:
	/**
	*	@brief Checks the status of the active framebuffer.
	*/
	bool checkFBOstate();

public:
	/**
	*	@brief Constructor for a framebuffer of any size.
	*/
	FBO(const uint16_t width, const uint16_t height);

	/**
	*	@brief Destructor.
	*/
	virtual ~FBO();

	/**
	*	@brief Binds the FBO so it is now active.
	*/
	virtual bool bindFBO() = 0;

	/**
	*	@brief ID provided by OpenGL for this framebuffer.
	*/
	virtual GLuint getIdentifier() const { return _id; }

	/**
	*	@brief Size of framebuffer.
	*/
	ivec2 getSize() const { return _size; }

	/**
	*	@brief Modifies the size specified in textures related to framebuffer. To be defined.
	*	@param width New width.
	*	@param height New height.
	*/
	virtual void modifySize(const uint16_t width, const uint16_t height);
};

