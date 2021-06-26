#pragma once

#include "Graphics/Application/RenderingParameters.h"
#include "Graphics/Core/FBO.h"

/**
*	@file SSAOFBO.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 11/29/2020
*/

/**
*	@brief FBO which wraps the G-buffer textures for ambient occlusion.
*/
class SSAOFBO: public FBO
{
protected:
	const vec3 BLACK_BACKGROUND = vec3(.0f);			//!< Clear color for normal and position textures
	const unsigned DEFAULT_SIZE = 2048;					//!< Default width / height

protected:
	GLuint					_gBufferFBO[3];				//!< 
	GLuint					_colorBuffer[3];			//!< Albedo, position and normal
	GLuint					_depthTexture[3];			//!< Identifier on GPU of depth texture for main FBO

	GLuint					_multisampledFBO;			//!<
	GLuint					_mColorBuffer;			//!<
	GLuint					_mDepthBuffer;				//!< Multisampling buffers

	GLuint					_ssaoFramebufferID;			//!< 
	GLuint					_ssaoColorBuffer;			//!< 

	RenderingParameters*	_rendParams;

protected:
	/**
	*	@brief Writes an image in file system in an isolated thread, so the application doesn't get stuck.
	*	@param pixels Pixels of image to be written.
	*	@param filename Path where the image must be written.
	*	@param width Width of image.
	*	@param height Height of image.
	*/
	void threadedWriteImage(std::vector<GLubyte>* pixels, const std::string& filename, const uint16_t width, const uint16_t height);

public:
	/**
	*	@brief Constructor.
	*/
	SSAOFBO();

	/**
	*	@brief Destructor.
	*/
	virtual ~SSAOFBO();

	/**
	*	@brief Binds the framebuffer so it gets active.
	*/
	virtual bool bindFBO();

	/**
	*	@brief 
	*/
	bool bindGBufferFBO(const unsigned index);

	/**
	*	@brief 
	*/
	bool bindMultisamplingFBO();

	/**
	*	@brief
	*/
	bool bindSSAOFBO();

	/**
	*	@brief 
	*/
	void bindGBufferTextures(RenderingShader* shader);

	/**
	*	@brief
	*/
	void bindSSAOTexture(RenderingShader* shader);

	/**
	*	@brief Modifies the size of color textures.
	*	@param width New width.
	*	@param height New height.
	*/
	virtual void modifySize(const uint16_t width, const uint16_t height);

	/**
	*	@brief 
	*/
	void writeGBuffer(const unsigned index);
};

