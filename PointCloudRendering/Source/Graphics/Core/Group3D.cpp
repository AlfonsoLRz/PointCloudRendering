#include "stdafx.h"
#include "Group3D.h"

#include "Geometry/3D/Ray3D.h"
#include "Geometry/3D/TriangleMesh.h"
#include "Geometry/3D/Intersections3D.h"
#include "Graphics/Application/Renderer.h"
#include "Graphics/Application/RenderingParameters.h"
#include "Graphics/Core/OpenGLUtilities.h"
#include "Graphics/Core/ShaderList.h"
#include "Graphics/Core/VAO.h"
#include "Utilities/ChronoUtilities.h"

/// [Initialization of static attributes]
const GLuint	Group3D::BVH_BUILDING_RADIUS = 100;

/// [Public methods]

Group3D::Group3D(const mat4& modelMatrix):
	Model3D(modelMatrix, 1),									// Just in case we need to save some component properties
	_numClusters(0), _bvhVAO(nullptr), _staticGPUData(nullptr)
{
}

Group3D::~Group3D()
{
	for (Model3D* object : _objects)
	{
		delete object;
	}

	delete _bvhVAO;
	delete _staticGPUData;
}

void Group3D::addComponent(Model3D* object)
{
	_objects.push_back(object);
}

Group3D::StaticGPUData* Group3D::generateBVH(bool buildVisualization)
{
	VolatileGPUData* volatileGPUData;

	this->aggregateSSBOData(volatileGPUData, _staticGPUData);

	// BVH generation
	const unsigned radius = BVH_BUILDING_RADIUS;
	
	ComputeShader* findNeighborShader		= ShaderList::getInstance()->getComputeShader(RendEnum::FIND_BEST_NEIGHBOR);
	ComputeShader* clusterMergingShader		= ShaderList::getInstance()->getComputeShader(RendEnum::CLUSTER_MERGING);
	ComputeShader* reallocClustersShader	= ShaderList::getInstance()->getComputeShader(RendEnum::REALLOCATE_CLUSTERS);
	ComputeShader* endLoopCompShader		= ShaderList::getInstance()->getComputeShader(RendEnum::END_LOOP_COMPUTATIONS);

	// Prefix scan
	ComputeShader* reduceShader				= ShaderList::getInstance()->getComputeShader(RendEnum::REDUCE_PREFIX_SCAN);
	ComputeShader* downSweepShader			= ShaderList::getInstance()->getComputeShader(RendEnum::DOWN_SWEEP_PREFIX_SCAN);
	ComputeShader* resetPositionShader		= ShaderList::getInstance()->getComputeShader(RendEnum::RESET_LAST_POSITION_PREFIX_SCAN);

	// Compute shader execution data: groups and iteration control
	unsigned arraySize = _staticGPUData->_numTriangles, startIndex = arraySize, finishBit = 0, iteration, numExec, numThreads, startThreads;
	int numGroups, numGroups2Log;
	const int maxGroupSize = ComputeShader::getMaxGroupSize();

	// Prepare buffers for GPU
	BVHCluster* outCluster = new BVHCluster[arraySize];					// We declare an input buffer instead of asking for a GPU one cause we've proved it's faster

	// Compact cluster buffer support
	GLuint* currentPosBuffer = new GLuint[arraySize], *currentPosBufferOut = new GLuint[arraySize];
	std::iota(currentPosBufferOut, currentPosBufferOut + arraySize, 0);

	GLuint coutBuffer				= volatileGPUData->_tempClusterSSBO;													// Swapped during loop => not const
	GLuint cinBuffer				= ComputeShader::setReadBuffer(outCluster, arraySize);
	GLuint inCurrentPosition		= ComputeShader::setReadBuffer(currentPosBuffer, arraySize);		// Position of compact buffer where a cluster is saved
	GLuint outCurrentPosition		= ComputeShader::setReadBuffer(currentPosBufferOut, arraySize);
	const GLuint neighborIndex		= ComputeShader::setWriteBuffer(GLuint(), arraySize);				// Nearest neighbor search	
	const GLuint prefixScan			= ComputeShader::setWriteBuffer(GLuint(), arraySize);				// Final position of each valid cluster for the next loop iteration
	const GLuint validCluster		= ComputeShader::setWriteBuffer(GLuint(), arraySize);				// Clusters which takes part of next loop iteration
	const GLuint mergedCluster		= ComputeShader::setWriteBuffer(GLuint(), arraySize);				// A merged cluster is always valid, but the opposite situation is not fitting
	const GLuint numNodesCount		= ComputeShader::setReadData(arraySize);							// Number of currently added nodes, which increases as the clusters are merged
	const GLuint arraySizeCount		= ComputeShader::setWriteBuffer(GLuint(), 1);

	while (arraySize > 1)
	{
		// Binary tree and whole array group sizes and iteration boundaries
		numGroups		= ComputeShader::getNumGroups(arraySize);
		startThreads	= std::ceil(arraySize / 2.0f);
		numExec			= std::ceil(std::log2(arraySize));
		numGroups2Log	= ComputeShader::getNumGroups(startThreads);

		std::vector<GLuint> threadCount{ startThreads };				// Thread sizes are repeated on reduce and sweep down phases
		threadCount.reserve(numExec);

		std::swap(coutBuffer, cinBuffer);
		std::swap(inCurrentPosition, outCurrentPosition);

		findNeighborShader->bindBuffers(std::vector<GLuint>{ cinBuffer, neighborIndex });
		findNeighborShader->use();
		findNeighborShader->setUniform("arraySize", arraySize);
		findNeighborShader->setUniform("radius", radius);
		findNeighborShader->execute(numGroups, 1, 1, maxGroupSize, 1, 1);

		clusterMergingShader->bindBuffers(std::vector<GLuint>{ cinBuffer, _staticGPUData->_clusterSSBO, neighborIndex, validCluster, mergedCluster, 
															   prefixScan, inCurrentPosition, numNodesCount });
		clusterMergingShader->use();
		clusterMergingShader->setUniform("arraySize", arraySize);
		clusterMergingShader->execute(numGroups, 1, 1, maxGroupSize, 1, 1);

		// FIRST STEP: build a binary tree with a summatory of the array
		reduceShader->bindBuffers(std::vector<GLuint> { prefixScan });
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

		// SECOND STEP: set last position to zero, its faster to do it in GPU than retrieve the array in CPU, modify and write it again to GPU
		resetPositionShader->bindBuffers(std::vector<GLuint> { prefixScan });
		resetPositionShader->use();
		resetPositionShader->setUniform("arraySize", arraySize);
		resetPositionShader->execute(1, 1, 1, 1, 1, 1);

		// THIRD STEP: build tree back to first level and compute final summatory
		downSweepShader->bindBuffers(std::vector<GLuint> { prefixScan });
		downSweepShader->use();
		downSweepShader->setUniform("arraySize", arraySize);

		iteration = threadCount.size() - 2;
		while (iteration >= 0 && iteration < numExec)
		{
			downSweepShader->setUniform("iteration", iteration);
			downSweepShader->setUniform("numThreads", threadCount[iteration--]);
			downSweepShader->execute(numGroups2Log, 1, 1, maxGroupSize, 1, 1);
		}

		reallocClustersShader->bindBuffers(std::vector<GLuint>{ cinBuffer, coutBuffer, validCluster, prefixScan, inCurrentPosition, outCurrentPosition });
		reallocClustersShader->use();
		reallocClustersShader->setUniform("arraySize", arraySize);
		reallocClustersShader->execute(numGroups, 1, 1, maxGroupSize, 1, 1);

		// Updates cluster size
		endLoopCompShader->bindBuffers(std::vector<GLuint>{ arraySizeCount, prefixScan, validCluster });
		endLoopCompShader->use();
		endLoopCompShader->setUniform("arraySize", arraySize);
		endLoopCompShader->execute(1, 1, 1, 1, 1, 1); 

		arraySize = endLoopCompShader->readData(arraySizeCount, GLuint())[0];
	}

	// Wait for the end of compute shaders
	BVHCluster* clusterData = ComputeShader::readData(_staticGPUData->_clusterSSBO, BVHCluster());
	volatileGPUData->_cluster = std::move(std::vector<BVHCluster>(clusterData, clusterData + _staticGPUData->_numTriangles * 2 - 1));				

	// Free buffers from GPU
	GLuint toDeleteBuffers[] = { coutBuffer, cinBuffer, inCurrentPosition, outCurrentPosition, neighborIndex,
							     prefixScan, validCluster, mergedCluster, numNodesCount, arraySizeCount };
	glDeleteBuffers(sizeof(toDeleteBuffers) / sizeof(GLuint), toDeleteBuffers);

	// CG Visualization
	if (buildVisualization)
	{
		this->buildBVHVAO(volatileGPUData);
	}

	// Free dynamic memory
	delete[]	outCluster;
	delete[]	currentPosBuffer;
	delete[]	currentPosBufferOut;

	return _staticGPUData;
}

Model3D::ModelComponent* Group3D::getModelComponent(unsigned id)
{
	return _globalModelComp[id];
}

bool Group3D::load(const mat4& modelMatrix)
{
	bool success = true;
	const mat4& mMatrix = modelMatrix * _modelMatrix;

	for (Model3D* model : _objects)
	{
		success &= model->load(mMatrix);
	}

	return success;
}

void Group3D::registerModelComponent(ModelComponent* modelComp)
{
	unsigned id = _globalModelComp.size();

	_globalModelComp.push_back(modelComp);
	modelComp->_id = id;
	modelComp->assignModelCompIDFaces();
}

void Group3D::registerModelComponentGroup(Group3D* group)
{
	for (Model3D* model : _objects)
	{
		model->registerModelComponentGroup(group);
	}
}

void Group3D::registerScene()
{
	for (Model3D* model : _objects)
	{
		model->registerModelComponentGroup(this);
	}
}

// --------------------------- Rendering ----------------------------------

void Group3D::drawAsLines(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix)
{
	this->setShaderUniforms(shader, shaderType, matrix);

	for (Model3D* object : _objects)
	{
		object->drawAsLines(shader, shaderType, matrix);
	}
}

void Group3D::drawAsPoints(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix)
{
	this->setShaderUniforms(shader, shaderType, matrix);

	for (Model3D* object : _objects)
	{
		object->drawAsPoints(shader, shaderType, matrix);
	}
}

void Group3D::drawAsTriangles(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix)
{
	Material* material = _modelComp[0]->_material;
	this->setShaderUniforms(shader, shaderType, matrix);

	for (Model3D* object : _objects)
	{
		if (material)
		{
			material->applyMaterial(shader);
		}

		object->drawAsTriangles(shader, shaderType, matrix);
	}
}

void Group3D::drawAsReflectiveTriangles(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix, Camera* camera, const GLuint lightIndex)
{
	Material* material = _modelComp[0]->_material;
	this->setShaderUniforms(shader, shaderType, matrix);

	for (Model3D* object : _objects)
	{
		object->drawAsReflectiveTriangles(shader, shaderType, matrix, camera);
	}
}

void Group3D::drawAsTriangles4Shadows(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix)
{
	this->setShaderUniforms(shader, shaderType, matrix);

	for (Model3D* object : _objects)
	{
		object->drawAsTriangles4Shadows(shader, shaderType, matrix);
	}
}

void Group3D::drawBVH(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix)
{
	RenderingParameters* rendParams = Renderer::getInstance()->getRenderingParameters();

	if (_bvhVAO)
	{
		this->setShaderUniforms(shader, shaderType, matrix);
		shader->setUniform("vColor", rendParams->_bvhWireframeColor);

		_bvhVAO->drawObject(RendEnum::IBOTypes::IBO_WIREFRAME, GL_LINES, rendParams->_bvhNodesPercentage * _numClusters * BVH_NODE_INDICES);		// 12 lines x 3 indices
	}
}

/// [Protected methods]

void Group3D::aggregateSSBOData(VolatileGPUData*& volatileGPUData, StaticGPUData*& staticGPUData)
{
	VolatileGroupData* groupData = new VolatileGroupData;
	unsigned numVertices = 0, numTriangles = 0;
	volatileGPUData = new VolatileGPUData;
	staticGPUData = new StaticGPUData;

	// We need to gather all the geometry and topology from the scene
	// First step: count vertices and faces to resize arrays only 1 time

	for (ModelComponent* modelComp : _globalModelComp)
	{
		numVertices += modelComp->_geometry.size();
		numTriangles += modelComp->_topology.size();
	}

	//this->writeModelComponentsPly();

	groupData->_geometry.resize(numVertices);
	groupData->_triangleMesh.resize(numTriangles);
	groupData->_meshData.resize(_globalModelComp.size());

	// Second step: gather geometry and topology
	unsigned currentGeometry = 0, currentTopology = 0;

	for (ModelComponent* modelComp : _globalModelComp)
	{
		numVertices = modelComp->_geometry.size();

		std::copy(modelComp->_geometry.begin(), modelComp->_geometry.end(), groupData->_geometry.begin() + currentGeometry);
		std::copy(modelComp->_topology.begin(), modelComp->_topology.end(), groupData->_triangleMesh.begin() + currentTopology);

		groupData->_meshData[modelComp->_id]._startIndex = currentGeometry;
		groupData->_meshData[modelComp->_id]._numVertices = numVertices;

		currentGeometry += numVertices;
		currentTopology += modelComp->_topology.size();

		modelComp->releaseMemory();
	}

	// Compute scene AABB once the geometry and topology is all given in a row
	_staticGPUData->_numTriangles		= groupData->_triangleMesh.size();
	this->_aabb							= this->computeAABB(groupData);

	staticGPUData->_groupGeometrySSBO	= ComputeShader::setReadBuffer(groupData->_geometry, GL_STATIC_DRAW);
	staticGPUData->_groupMeshSSBO		= ComputeShader::setReadBuffer(groupData->_meshData, GL_STATIC_DRAW);
	staticGPUData->_groupTopologySSBO	= ComputeShader::setReadBuffer(groupData->_triangleMesh, GL_STATIC_DRAW);
	const GLuint mortonCodes			= this->computeMortonCodes();
	const GLuint sortedIndices			= this->sortFacesByMortonCode(mortonCodes);
	
	this->buildClusterBuffer(volatileGPUData, sortedIndices);

	delete groupData;
}

void Group3D::buildBVHVAO(VolatileGPUData* gpuData)
{
	if (!_bvhVAO)
	{
		_bvhVAO = Primitives::getCubesVAO(gpuData->_cluster, _numClusters = gpuData->_cluster.size());
	}
}

void Group3D::buildClusterBuffer(VolatileGPUData* gpuData, const GLuint sortedFaces)
{
	ComputeShader* buildClusterShader = ShaderList::getInstance()->getComputeShader(RendEnum::BUILD_CLUSTER_BUFFER);

	const unsigned arraySize		= _staticGPUData->_numTriangles;
	const unsigned clusterSize		= _staticGPUData->_numTriangles * 2 - 1;				// We'll only fill arraySize clusters
	const int numGroups				= ComputeShader::getNumGroups(arraySize);

	BVHCluster* clusterData			= new BVHCluster[clusterSize], *tempClusterData = new BVHCluster[arraySize];
	_staticGPUData->_clusterSSBO	= ComputeShader::setReadBuffer(clusterData, clusterSize);
	gpuData->_tempClusterSSBO		= ComputeShader::setReadBuffer(tempClusterData, arraySize);

	buildClusterShader->bindBuffers(std::vector<GLuint> { _staticGPUData->_groupTopologySSBO, sortedFaces, _staticGPUData->_clusterSSBO, gpuData->_tempClusterSSBO });
	buildClusterShader->use();
	buildClusterShader->setUniform("arraySize", arraySize);
	buildClusterShader->execute(numGroups, 1, 1, ComputeShader::getMaxGroupSize(), 1, 1);

	glDeleteBuffers(1, &sortedFaces);

	delete[] clusterData;
	delete[] tempClusterData;
}

AABB Group3D::computeAABB(VolatileGroupData* groupData)
{
	ComputeShader* computeAABBShader	= ShaderList::getInstance()->getComputeShader(RendEnum::COMPUTE_GROUP_AABB);

	const GLuint triangleBufferID		= ComputeShader::setReadBuffer(groupData->_triangleMesh, GL_DYNAMIC_DRAW);
	const GLuint maxPointBufferID		= ComputeShader::setWriteBuffer(vec4(), 1, GL_DYNAMIC_DRAW);
	const GLuint minPointBufferID		= ComputeShader::setWriteBuffer(vec4(), 1, GL_DYNAMIC_DRAW);

	unsigned arraySize		= _staticGPUData->_numTriangles;
	const int numExec		= std::ceil(std::log2(arraySize));
	unsigned numThreads		= std::ceil(arraySize / 2.0f);
	int numGroups			= ComputeShader::getNumGroups(numThreads);

	computeAABBShader->bindBuffers(std::vector<GLuint> { triangleBufferID, maxPointBufferID, minPointBufferID });
	computeAABBShader->use();
	computeAABBShader->setUniform("arraySize", arraySize);

	for (unsigned i = 0; i < numExec; ++i)
	{
		computeAABBShader->setUniform("iteration", i);
		computeAABBShader->setUniform("numThreads", numThreads);
		computeAABBShader->execute(numGroups, 1, 1, ComputeShader::getMaxGroupSize(), 1, 1);

		numThreads = std::ceil(numThreads / 2.0f);
		numGroups = ComputeShader::getNumGroups(numThreads);
	}

	vec4* pMaxPoint = computeAABBShader->readData(maxPointBufferID, vec4()), *pMinPoint = computeAABBShader->readData(minPointBufferID, vec4());
	vec3 maxPoint = *pMaxPoint, minPoint = *pMinPoint;

	glDeleteBuffers(1, &triangleBufferID);
	glDeleteBuffers(1, &maxPointBufferID);
	glDeleteBuffers(1, &minPointBufferID);

	return AABB(minPoint, maxPoint);
}

GLuint Group3D::computeMortonCodes()
{
	ComputeShader* computeMortonShader = ShaderList::getInstance()->getComputeShader(RendEnum::COMPUTE_MORTON_CODES);

	const unsigned arraySize = _staticGPUData->_numTriangles;
	const int numGroups = ComputeShader::getNumGroups(arraySize);
	const GLuint mortonCodeBuffer = ComputeShader::setWriteBuffer(unsigned(), arraySize);

	computeMortonShader->bindBuffers(std::vector<GLuint> { _staticGPUData->_groupTopologySSBO, mortonCodeBuffer });
	computeMortonShader->use();
	computeMortonShader->setUniform("arraySize", arraySize);
	computeMortonShader->setUniform("sceneMaxBoundary", _aabb.max());
	computeMortonShader->setUniform("sceneMinBoundary", _aabb.min());
	computeMortonShader->execute(numGroups, 1, 1, ComputeShader::getMaxGroupSize(), 1, 1);

	return mortonCodeBuffer;
}

GLuint Group3D::sortFacesByMortonCode(const GLuint mortonCodes)
{
	ComputeShader* bitMaskShader			= ShaderList::getInstance()->getComputeShader(RendEnum::BIT_MASK_RADIX_SORT);
	ComputeShader* reduceShader				= ShaderList::getInstance()->getComputeShader(RendEnum::REDUCE_PREFIX_SCAN);
	ComputeShader* downSweepShader			= ShaderList::getInstance()->getComputeShader(RendEnum::DOWN_SWEEP_PREFIX_SCAN);
	ComputeShader* resetPositionShader		= ShaderList::getInstance()->getComputeShader(RendEnum::RESET_LAST_POSITION_PREFIX_SCAN);
	ComputeShader* reallocatePositionShader = ShaderList::getInstance()->getComputeShader(RendEnum::REALLOCATE_RADIX_SORT);

	const unsigned numBits	= 30;	// 10 bits per coordinate (3D)
	unsigned arraySize		= _staticGPUData->_numTriangles, currentBits = 0;
	const int numGroups		= ComputeShader::getNumGroups(arraySize);
	const int maxGroupSize	= ComputeShader::getMaxGroupSize();
	GLuint* indices			= new GLuint[arraySize];

	// Binary tree parameters
	const unsigned startThreads = std::ceil(arraySize / 2.0f);
	const int numExec			= std::ceil(std::log2(arraySize));
	const int numGroups2Log		= ComputeShader::getNumGroups(startThreads);
	unsigned numThreads			= 0, iteration;

	// Fill indices array from zero to arraySize - 1
	for (int i = 0; i < arraySize; ++i) { indices[i] = i; }

	GLuint indicesBufferID_1, indicesBufferID_2, pBitsBufferID, nBitsBufferID, positionBufferID;
	indicesBufferID_1	= ComputeShader::setWriteBuffer(GLuint(), arraySize);
	indicesBufferID_2	= ComputeShader::setReadBuffer(indices, arraySize);					// Substitutes indicesBufferID_1 for the next iteration
	pBitsBufferID		= ComputeShader::setWriteBuffer(GLuint(), arraySize);
	nBitsBufferID		= ComputeShader::setWriteBuffer(GLuint(), arraySize);
	positionBufferID	= ComputeShader::setWriteBuffer(GLuint(), arraySize);

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

		iteration = threadCount.size() - 2;
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

void Group3D::writeModelComponentsPly()
{
	for (ModelComponent* modelComp: _globalModelComp)
	{
		std::filebuf fileBufferBinary;
		fileBufferBinary.open(modelComp->_name + ".ply", std::ios::out | std::ios::binary);

		std::ostream outstreamBinary(&fileBufferBinary);
		if (outstreamBinary.fail()) throw std::runtime_error("Failed to open " + modelComp->_name + ".ply");

		tinyply::PlyFile plyFile;

		std::vector<vec3> position, normal;
		std::vector<vec2> textCoord;
		std::vector<uvec3> triangleMesh;

		for (int i = 0; i < modelComp->_geometry.size(); ++i)
		{
			position.push_back(modelComp->_geometry[i]._position);
			normal.push_back(modelComp->_geometry[i]._normal);
			textCoord.push_back(modelComp->_geometry[i]._textCoord);
		}

		for (int i = 0; i < modelComp->_topology.size(); ++i)
		{
			triangleMesh.push_back(modelComp->_topology[i]._vertices);
		}

		plyFile.add_properties_to_element("vertex", { "x", "y", "z" }, tinyply::Type::FLOAT32, position.size(), reinterpret_cast<uint8_t*>(position.data()), tinyply::Type::INVALID, 0);
		plyFile.add_properties_to_element("vertex", { "nx", "ny", "nz" }, tinyply::Type::FLOAT32, normal.size(), reinterpret_cast<uint8_t*>(normal.data()), tinyply::Type::INVALID, 0);
		plyFile.add_properties_to_element("vertex", { "s", "t" }, tinyply::Type::FLOAT32, textCoord.size(), reinterpret_cast<uint8_t*>(textCoord.data()), tinyply::Type::INVALID, 0);
		plyFile.add_properties_to_element("face", { "vertex_index" }, tinyply::Type::UINT32, triangleMesh.size(), reinterpret_cast<uint8_t*>(triangleMesh.data()), tinyply::Type::UINT8, 3);
		
		plyFile.write(outstreamBinary, true);
	}
}

/// VolatileGPUData

Group3D::VolatileGPUData::VolatileGPUData() : _tempClusterSSBO(-1), _mortonCodesSSBO(-1)
{
}

Group3D::VolatileGPUData::~VolatileGPUData()
{
	glDeleteBuffers(1, &_mortonCodesSSBO);
	glDeleteBuffers(1, &_tempClusterSSBO);
}

// StaticGPUData

Group3D::StaticGPUData::StaticGPUData() : _groupGeometrySSBO(-1), _groupTopologySSBO(-1), _groupMeshSSBO(-1), _clusterSSBO(-1), _numTriangles(0)
{
}

Group3D::StaticGPUData::~StaticGPUData()
{
	// Delete buffers
	GLuint toDeleteBuffers[] = { _groupGeometrySSBO, _groupTopologySSBO, _groupMeshSSBO, _clusterSSBO };
	glDeleteBuffers(sizeof(toDeleteBuffers) / sizeof(GLuint), toDeleteBuffers);
}
