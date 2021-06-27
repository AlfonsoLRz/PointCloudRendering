#pragma once

#include "Graphics/Core/PointCloud.h"

/**
*	@file PointCloudAggregator.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 25/06/2021
*/

/**
*	@brief 
*/
class PointCloudAggregator
{
protected:
	PointCloud*				_pointCloud;
	
	// SSBO
	std::vector<GLuint>		_pointCloudSSBO;
	std::vector<GLuint>		_pointCloudChunkSize;
	GLuint					_depthBufferSSBO;

	// OpenGL Texture
	GLuint					_textureID;

	// Shaders
	ComputeShader*			_projectionShader;
	ComputeShader*			_resetDepthBufferShader;
	ComputeShader*			_storeTexture;

	// Window
	RenderingParameters*	_renderingParameters;
	uvec2					_windowSize;
	bool					_changedWindowSize;

protected:
	/**
	*	@return
	*/
	static unsigned getAllowedNumberOfPoints();

protected:
	/**
	*	@brief Binds the texture. 
	*/
	void bindTexture();
	
	/**
	*	@brief Projects the point cloud SSBOs into a window plane. 
	*/
	void projectPointCloud(const mat4& projectionMatrix);
	
	/**
	*	@brief  
	*/
	void updateWindowBuffers();

	/**
	*	@brief Writes colors from the point cloud into a texture. 
	*/
	void writeColorsTexture();

	/**
	*	@brief Transfer point cloud information to GPU. 
	*/
	void writePointCloudGPU();

public:
	/**
	*	@brief  
	*/
	PointCloudAggregator(PointCloud* pointCloud);

	/**
	*	@brief Destructor. 
	*/
	virtual ~PointCloudAggregator();

	/**
	*	@brief Modifies size of buffer affected by the size of the window. 
	*/
	void changedSize(const uint16_t width, const uint16_t height);

	/**
	*	@return Identifier of image texture with point cloud colors. 
	*/
	GLuint getTexture() { return _textureID; }

	/**
	*	@brief Triggers the rendering of a new frame. 
	*/
	void render(const mat4& projectionMatrix);
};

