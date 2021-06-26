#include "stdafx.h"
#include "PointCloudAggregator.h"

#include "Graphics/Core/OpenGLUtilities.h"
#include "Graphics/Core/ShaderList.h"
#include "Interface/Window.h"

// [Public methods]

PointCloudAggregator::PointCloudAggregator(PointCloud* pointCloud) :
	_pointCloud(pointCloud), _textureID(-1), _depthBufferSSBO(-1)
{
	ShaderList* shaderList	= ShaderList::getInstance();
	Window* window			= Window::getInstance();

	_resetDepthBufferShader = shaderList->getComputeShader(RendEnum::RESET_DEPTH_BUFFER_SHADER);
	_projectionShader		= shaderList->getComputeShader(RendEnum::PROJECTION_SHADER);
	_storeTexture			= shaderList->getComputeShader(RendEnum::STORE_TEXTURE_SHADER);

	_windowSize				= window->getSize();

	_depthBufferSSBO		= ComputeShader::setWriteBuffer(uint64_t(), _windowSize.x * _windowSize.y, GL_DYNAMIC_DRAW);

	// Window texture
	glGenTextures(1, &_textureID);
	glBindTexture(GL_TEXTURE_2D, _textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _windowSize.x, _windowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glGenerateMipmap(GL_TEXTURE_2D);

	this->writePointCloudGPU();
}

PointCloudAggregator::~PointCloudAggregator()
{
	for (GLuint ssbo: _pointCloudSSBO)
	{
		glDeleteBuffers(1, &ssbo);
	}

	glDeleteBuffers(1, &_depthBufferSSBO);
	glDeleteTextures(1, &_textureID);
}

void PointCloudAggregator::changedSize(const uint16_t width, const uint16_t height)
{
	_windowSize = ivec2(width, height);
	_changedWindowSize = true;
}

void PointCloudAggregator::render(const mat4& projectionMatrix)
{
	if (_changedWindowSize)
	{
		this->updateWindowBuffers();
		_changedWindowSize = false;
	}

	//this->projectPointCloud(projectionMatrix);
	this->writeColorsTexture();
}

// [Protected methods]

unsigned PointCloudAggregator::getAllowedNumberOfPoints()
{
	unsigned pointSize = sizeof(PointCloud::PointModel);
	GLint limitedMemory;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &limitedMemory);

	return std::floor(limitedMemory / pointSize);
}

void PointCloudAggregator::bindTexture()
{
	glActiveTexture(GL_TEXTURE0);
	glBindImageTexture(0, _textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
}

void PointCloudAggregator::projectPointCloud(const mat4& projectionMatrix)
{
	unsigned chunk = 0, accumSize = 0;
	const int numGroupsImage = ComputeShader::getNumGroups(_windowSize.x * _windowSize.y);
	
	// 1. Fill buffer of 64 bits with UINT64_MAX, i.e. the null index is UINT_MAX
	_resetDepthBufferShader->bindBuffers(std::vector<GLuint> { _depthBufferSSBO });
	_resetDepthBufferShader->use();
	_resetDepthBufferShader->setUniform("windowSize", _windowSize);
	_resetDepthBufferShader->execute(numGroupsImage, 1, 1, ComputeShader::getMaxGroupSize(), 1, 1);
	
	for (GLuint pointsSSBO: _pointCloudSSBO)
	{
		const unsigned numPoints = _pointCloudChunkSize[chunk];
		const int numGroupsPoints = ComputeShader::getNumGroups(numPoints);

		// 2. Transform points and use atomicMin to retrieve the nearest point
		_projectionShader->bindBuffers(std::vector<GLuint> { _depthBufferSSBO, pointsSSBO });
		_projectionShader->use();
		_projectionShader->setUniform("cameraMatrix", projectionMatrix);
		_projectionShader->setUniform("numPoints", numPoints);
		_projectionShader->setUniform("offset", 0);
		_projectionShader->setUniform("windowSize", _windowSize);
		_projectionShader->execute(numGroupsPoints, 1, 1, ComputeShader::getMaxGroupSize(), 1, 1);

		//uint64_t* depthBufferp = ComputeShader::readData(_depthBufferSSBO, uint64_t());
		//std::vector<uint64_t> depthBuffer = std::vector<uint64_t>(depthBufferp, depthBufferp + _windowSize.x * _windowSize.y);

		accumSize += _pointCloudChunkSize[chunk++];
	}
}

void PointCloudAggregator::updateWindowBuffers()
{
	ComputeShader::updateWriteBuffer(_depthBufferSSBO, uint64_t(), _windowSize.x * _windowSize.y, GL_DYNAMIC_DRAW);

	// Update size of texture
	glBindTexture(GL_TEXTURE_2D, _textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _windowSize.x, _windowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
}

void PointCloudAggregator::writeColorsTexture()
{
	const int numGroupsImage = ComputeShader::getNumGroups(_windowSize.x * _windowSize.y);
	
	_storeTexture->bindBuffers(std::vector<GLuint> { /*_depthBufferSSBO, _pointCloudSSBO[0]*/ });
	_storeTexture->use();
	this->bindTexture();
	_storeTexture->setUniform("texImage", GLint(0));
	_storeTexture->setUniform("windowSize", _windowSize);
	_storeTexture->execute(numGroupsImage, 1, 1, ComputeShader::getMaxGroupSize(), 1, 1);
}

void PointCloudAggregator::writePointCloudGPU()
{
	unsigned currentNumPoints, leftPoints = _pointCloud->getNumberOfPoints();
	const unsigned numPoints = std::min(this->getAllowedNumberOfPoints(), _pointCloud->getNumberOfPoints());
	std::vector<PointCloud::PointModel>* points = _pointCloud->getPoints();

	while (leftPoints > 0)
	{
		currentNumPoints = std::min(numPoints, leftPoints);

		const GLuint pointBufferSSBO = ComputeShader::setReadBuffer(&(points->at(points->size() - leftPoints)), currentNumPoints, GL_DYNAMIC_DRAW);
		_pointCloudSSBO.push_back(pointBufferSSBO);
		_pointCloudChunkSize.push_back(currentNumPoints);

		leftPoints -= currentNumPoints;
	}
}
