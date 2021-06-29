#include "stdafx.h"
#include "PointCloudAggregator.h"

#include "Graphics/Application/Renderer.h"
#include "Graphics/Core/OpenGLUtilities.h"
#include "Graphics/Core/ShaderList.h"
#include "Interface/Window.h"

// [Public methods]

PointCloudAggregator::PointCloudAggregator(PointCloud* pointCloud) :
	_pointCloud(pointCloud), _textureID(-1), _depthBufferSSBO(-1)
{
	ShaderList* shaderList	= ShaderList::getInstance();
	Window* window			= Window::getInstance();

	_renderingParameters	= Renderer::getInstance()->getRenderingParameters();
	
	_resetDepthBufferShader = shaderList->getComputeShader(RendEnum::RESET_DEPTH_BUFFER_SHADER);
	_projectionShader		= shaderList->getComputeShader(RendEnum::PROJECTION_SHADER);
	_storeTexture			= shaderList->getComputeShader(RendEnum::STORE_TEXTURE_SHADER);
	_supportBuffer.resize(this->getAllowedNumberOfPoints());

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

	this->projectPointCloud(projectionMatrix);
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
	glBindImageTexture(0, _textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
}

GLuint PointCloudAggregator::calculateMortonCodes(const GLuint pointsSSBO, unsigned numPoints)
{
	ComputeShader* computeMortonShader = ShaderList::getInstance()->getComputeShader(RendEnum::COMPUTE_MORTON_CODES_PCL);

	const int numGroups = ComputeShader::getNumGroups(numPoints);
	const GLuint mortonCodeBuffer = ComputeShader::setWriteBuffer(unsigned(), numPoints);

	computeMortonShader->bindBuffers(std::vector<GLuint> { pointsSSBO, mortonCodeBuffer });
	computeMortonShader->use();
	computeMortonShader->setUniform("arraySize", numPoints);
	computeMortonShader->setUniform("sceneMaxBoundary", _pointCloud->getAABB().max());
	computeMortonShader->setUniform("sceneMinBoundary", _pointCloud->getAABB().min());
	computeMortonShader->execute(numGroups, 1, 1, ComputeShader::getMaxGroupSize(), 1, 1);

	return mortonCodeBuffer;
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
		_projectionShader->setUniform("windowSize", _windowSize);
		_projectionShader->execute(numGroupsPoints, 1, 1, ComputeShader::getMaxGroupSize(), 1, 1);

		accumSize += _pointCloudChunkSize[chunk++];
	}
}

void PointCloudAggregator::sortPoints(const GLuint pointsSSBO, unsigned numPoints)
{
	const GLuint pointCodeSSBO	= this->calculateMortonCodes(pointsSSBO, numPoints);

	const GLuint indicesBufferSSBO = this->sortFacesByMortonCode(pointCodeSSBO, numPoints);
	GLuint* indices = ComputeShader::readData(indicesBufferSSBO, GLuint());
	std::vector<GLuint> bufferIndices = std::vector<GLuint>(indices, indices + numPoints);

	std::vector<PointCloud::PointModel>* previousPoints = _pointCloud->getPoints();

	for (int pointIdx = 0; pointIdx < numPoints; ++pointIdx)
	{
		_supportBuffer.at(pointIdx) = previousPoints->at(indices[pointIdx]);
	}

	ComputeShader::updateReadBuffer(pointsSSBO, _supportBuffer.data(), numPoints, GL_STATIC_DRAW);

	glDeleteBuffers(1, &indicesBufferSSBO);
}

GLuint PointCloudAggregator::sortFacesByMortonCode(const GLuint mortonCodes, unsigned numPoints)
{
	ComputeShader* bitMaskShader = ShaderList::getInstance()->getComputeShader(RendEnum::BIT_MASK_RADIX_SORT);
	ComputeShader* reduceShader = ShaderList::getInstance()->getComputeShader(RendEnum::REDUCE_PREFIX_SCAN);
	ComputeShader* downSweepShader = ShaderList::getInstance()->getComputeShader(RendEnum::DOWN_SWEEP_PREFIX_SCAN);
	ComputeShader* resetPositionShader = ShaderList::getInstance()->getComputeShader(RendEnum::RESET_LAST_POSITION_PREFIX_SCAN);
	ComputeShader* reallocatePositionShader = ShaderList::getInstance()->getComputeShader(RendEnum::REALLOCATE_RADIX_SORT);

	const unsigned numBits	= 30;			// 10 bits per coordinate (3D)
	unsigned arraySize		= numPoints;
	unsigned currentBits	= 0;
	const int numGroups		= ComputeShader::getNumGroups(arraySize);
	const int maxGroupSize	= ComputeShader::getMaxGroupSize();
	GLuint* indices			= new GLuint[arraySize];

	// Binary tree parameters
	const unsigned startThreads = unsigned(std::ceil(arraySize / 2.0f));
	const unsigned numExec = unsigned(std::ceil(std::log2(arraySize)));
	const unsigned numGroups2Log = unsigned(ComputeShader::getNumGroups(startThreads));
	unsigned numThreads = 0, iteration;

	// Fill indices array from zero to arraySize - 1
	for (int i = 0; i < arraySize; ++i) { indices[i] = i; }

	GLuint indicesBufferID_1, indicesBufferID_2, pBitsBufferID, nBitsBufferID, positionBufferID;
	indicesBufferID_1 = ComputeShader::setWriteBuffer(GLuint(), arraySize);
	indicesBufferID_2 = ComputeShader::setReadBuffer(indices, arraySize);					// Substitutes indicesBufferID_1 for the next iteration
	pBitsBufferID = ComputeShader::setWriteBuffer(GLuint(), arraySize);
	nBitsBufferID = ComputeShader::setWriteBuffer(GLuint(), arraySize);
	positionBufferID = ComputeShader::setWriteBuffer(GLuint(), arraySize);

	while (currentBits < numBits)
	{
		std::vector<GLuint> threadCount{ startThreads };
		threadCount.reserve(numExec);

		std::swap(indicesBufferID_1, indicesBufferID_2);							// indicesBufferID_2 is initialized with indices cause it's swapped here

		// FIRST STEP: BIT MASK, check if a morton code gives zero or one for a certain mask (iteration)
		unsigned bitMask = 1 << currentBits++;

		bitMaskShader->bindBuffers(std::vector<GLuint> { mortonCodes, indicesBufferID_1, pBitsBufferID, nBitsBufferID });
		bitMaskShader->use();
		bitMaskShader->setUniform("arraySize", arraySize);
		bitMaskShader->setUniform("bitMask", bitMask);
		bitMaskShader->execute(numGroups, 1, 1, maxGroupSize, 1, 1);

		// SECOND STEP: build a binary tree with a summatory of the array
		reduceShader->bindBuffers(std::vector<GLuint> { nBitsBufferID });
		reduceShader->use();
		reduceShader->setUniform("arraySize", arraySize);

		iteration = 0;
		while (iteration < numExec)
		{
			numThreads = threadCount[threadCount.size() - 1];

			reduceShader->setUniform("iteration", iteration++);
			reduceShader->setUniform("numThreads", numThreads);
			reduceShader->execute(numGroups2Log, 1, 1, maxGroupSize, 1, 1);

			threadCount.push_back(std::ceil(numThreads / 2.0f));
		}

		// THIRD STEP: set last position to zero, its faster to do it in GPU than retrieve the array in CPU, modify and write it again to GPU
		resetPositionShader->bindBuffers(std::vector<GLuint> { nBitsBufferID });
		resetPositionShader->use();
		resetPositionShader->setUniform("arraySize", arraySize);
		resetPositionShader->execute(1, 1, 1, 1, 1, 1);

		// FOURTH STEP: build tree back to first level and compute position of each bit
		downSweepShader->bindBuffers(std::vector<GLuint> { nBitsBufferID });
		downSweepShader->use();
		downSweepShader->setUniform("arraySize", arraySize);

		iteration = unsigned(threadCount.size()) - 2;
		while (iteration >= 0 && iteration < numExec)
		{
			downSweepShader->setUniform("iteration", iteration);
			downSweepShader->setUniform("numThreads", threadCount[iteration--]);
			downSweepShader->execute(numGroups2Log, 1, 1, maxGroupSize, 1, 1);
		}

		reallocatePositionShader->bindBuffers(std::vector<GLuint> { pBitsBufferID, nBitsBufferID, indicesBufferID_1, indicesBufferID_2 });
		reallocatePositionShader->use();
		reallocatePositionShader->setUniform("arraySize", arraySize);
		reallocatePositionShader->execute(numGroups, 1, 1, maxGroupSize, 1, 1);
	}

	glDeleteBuffers(1, &indicesBufferID_1);
	glDeleteBuffers(1, &pBitsBufferID);
	glDeleteBuffers(1, &nBitsBufferID);
	glDeleteBuffers(1, &positionBufferID);

	delete[] indices;

	return indicesBufferID_2;
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
	
	_storeTexture->bindBuffers(std::vector<GLuint> { _depthBufferSSBO });
	_storeTexture->use();
	this->bindTexture();
	_storeTexture->setUniform("backgroundColor", _renderingParameters->_backgroundColor);
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

		if (true)
		{
			this->sortPoints(pointBufferSSBO, currentNumPoints);
		}

		leftPoints -= currentNumPoints;
	}
}
