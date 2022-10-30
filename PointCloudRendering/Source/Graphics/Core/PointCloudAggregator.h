#pragma once

#include "Graphics/Core/PointCloud.h"

/**
*	@file PointCloudAggregator.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 25/06/2021
*/

typedef PointCloud::PointModel Point;

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
	GLuint					_depthBufferSSBO, _rawDepthBufferSSBO, _color01SSBO, _color02SSBO;
	GLuint					_SMSSBO;
	std::vector<Point>		_supportBuffer;

	// OpenGL Texture
	GLuint					_textureID;

	// Shaders
	ComputeShader*			_addColorsHQRShader;
	ComputeShader*			_projectionShader, *_projectionHQRShader;
	ComputeShader*			_resetDepthBufferShader, * _resetDepthBufferHQRShader;
	ComputeShader*			_storeTexture, *_storeHQRTexture;

	ComputeShader*			_gpuPatternShader, * _storeGpuPatternShader;

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
	*	@brief
	*/
	GLuint calculateMortonCodes(const GLuint pointsSSBO, unsigned numPoints);

	/**
	*	@brief  
	*/
	void deletePointCloudBuffers();
	
	/**
	*	@brief Projects the point cloud SSBOs into a window plane. 
	*/
	void projectPointCloud(const mat4& viewMatrix, const mat4& projectionMatrix);

	/**
	*	@brief Projects the point cloud SSBOs into a window plane.
	*/
	void projectPointCloudHQR(const mat4& viewMatrix, const mat4& projectionMatrix);

	/**
	*	@brief  
	*/
	void reducePointChunk(GLuint& pointsSSBO, const GLuint indexSSBO, unsigned& numPoints);

	/**
	*	@brief
	*/
	void sortPoints(const GLuint pointsSSBO, unsigned numPoints);

	/**
	*	@brief
	*/
	GLuint sortFacesByMortonCode(const GLuint mortonCodes, unsigned numPoints);
	
	/**
	*	@brief  
	*/
	void updateWindowBuffers();

	/**
	*	@brief Writes colors from the point cloud into a texture. 
	*/
	void writeColorsTexture();

	/**
	*	@brief Writes colors from the point cloud into a texture.
	*/
	void writeColorsTextureHQR();

	/**
	*	@brief Transfer point cloud information to GPU. 
	*/
	void writePointCloudGPU();

public:
	/**
	*	@brief  
	*/
	PointCloudAggregator();

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
	void render(const mat4& viewMatrix, const mat4& projectionMatrix);

	/**
	*	@brief 
	*/
	void renderGPUPattern();

	/**
	*	@brief
	*/
	void setPointCloud(PointCloud* pointCloud);
};

