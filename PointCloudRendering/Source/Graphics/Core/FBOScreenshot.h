#pragma once

#include "Graphics/Core/FBO.h"
#include "Graphics/Core/Texture.h"

/**
*	@file FBOScreenshot.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/26/2019
*/

/**
*	@brief Specialized framebuffer to take screenshots of the scene.
*/
class FBOScreenshot: public FBO
{
protected:
	GLuint _multisampledFBO, _colorBufferID;
	GLuint _mColorBufferID, _mDepthBufferID;						//!< Multisampling buffers

public:
	/**
	*	@brief Constructor.
	*	@param width Width of image we want to take.
	*	@param height Height of image we want to take.
	*/
	FBOScreenshot(const uint16_t width, const uint16_t height);

	/**
	*	@brief Destructor.
	*/
	virtual ~FBOScreenshot();

	/**
	*	@brief Binds the framebuffer so it gets active.
	*/
	virtual bool bindFBO();

	/**
	*	@brief ID provided by OpenGL for this framebuffer.
	*/
	virtual GLuint getIdentifier()  const { return _multisampledFBO; }

	/**
	*	@return Rendered screen as an image.
	*/
	Image* getImage() const;

	/**
	*	@brief Modifies the size specified in textures related to framebuffer. To be defined.
	*	@param width New width.
	*	@param height New height.
	*/
	virtual void modifySize(const uint16_t width, const uint16_t height);

	/**
	*	@brief Saves the captured scene in a file.
	*	@param filename Path of file.
	*/
	bool saveImage(const std::string& filename);
};

