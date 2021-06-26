#include "stdafx.h"
#include "PlanarSurface.h"

#include "Graphics/Core/ShaderList.h"
#include "Graphics/Core/VAO.h"
#include "Geometry/3D/TriangleMesh.h"
#include "Utilities/ChronoUtilities.h"

/// [Public methods]

PlanarSurface::PlanarSurface(const float width, const float depth, const uint16_t tilingH, const uint16_t tilingV, const float maxTextValH, const float maxTextValV, const mat4& modelMatrix): 
	Model3D(modelMatrix, 1),
	_width(width), _depth(depth),
	_tilingH(tilingH), _tilingV(tilingV),
	_maxTextValH(maxTextValH), _maxTextValV(maxTextValV)
{
}

PlanarSurface::~PlanarSurface()
{
}

void PlanarSurface::drawAsLines(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix)
{
	this->renderLines(shader, shaderType, matrix, _modelComp[0], GL_LINE_STRIP);
}

void PlanarSurface::drawAsTriangles(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix)
{
	this->renderTriangles(shader, shaderType, matrix, _modelComp[0], GL_TRIANGLE_STRIP);
}

void PlanarSurface::drawAsTriangles4Shadows(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix)
{
	this->renderTriangles4Shadows(shader, shaderType, matrix, _modelComp[0], GL_TRIANGLE_STRIP);
}

bool PlanarSurface::load(const mat4& modelMatrix)
{
	if (!_loaded)
	{
		this->generateGeometryTopology(modelMatrix);
		this->setVAOData();

		return _loaded = true;
	}

	return false;
}

/// [Protected methods]

void PlanarSurface::generateGeometryTopology(const mat4& modelMatrix)
{
	// Geometry constraints
	const float width_2		= _width / 2.0f,		depth_2		= _depth / 2.0f;
	const float tileWidth	= _width / _tilingH,	tileDepth	= _depth / _tilingV;

	// Topology constraints
	const unsigned numVertices	= (_tilingH + 1) * (_tilingV + 1);
	const unsigned numTriangles = _tilingH * _tilingV * 2;
	const unsigned numIndices	= _tilingH * (_tilingV + 1) * 2 + _tilingH;

	unsigned numGroups = 128;
	const GLuint workGroupSize_x = ComputeShader::getWorkGroupSize(numGroups, _tilingH + 1);
	const GLuint workGroupSize_y = ComputeShader::getWorkGroupSize(numGroups, _tilingV + 1);

	// FIRST PHASE: create mesh geometry and topology
	ModelComponent* modelComp = _modelComp[0];
	ComputeShader* shader = ShaderList::getInstance()->getComputeShader(RendEnum::PLANAR_SURFACE_GENERATION);

	GLuint modelBufferID, meshBufferID, rawMeshBufferID;
	modelBufferID = ComputeShader::setWriteBuffer(Model3D::VertexGPUData(), numVertices);
	meshBufferID = ComputeShader::setWriteBuffer(Model3D::FaceGPUData(), numTriangles);
	rawMeshBufferID = ComputeShader::setWriteBuffer(GLuint(), numIndices);

	shader->bindBuffers(std::vector<GLuint> { modelBufferID, rawMeshBufferID });
	shader->use();
	shader->setUniform("mModel", modelMatrix * _modelMatrix);
	shader->setUniform("gridSize", vec2(_tilingH, _tilingV));
	shader->setUniform("gridDim_2", vec2(width_2, depth_2));
	shader->setUniform("tileDim", vec2(tileWidth, tileDepth));
	shader->setUniform("maxTextCoord", vec2(_maxTextValH, _maxTextValV));
	shader->setUniform("restartPrimitiveIndex", Model3D::RESTART_PRIMITIVE_INDEX);
	if (modelComp->_material) modelComp->_material->applyMaterial4ComputeShader(shader);
	shader->execute(numGroups, numGroups, 1, workGroupSize_x, workGroupSize_y, 1);

	// SECOND PHASE: compute topology once geometry is fully computed
	numGroups = ComputeShader::getNumGroups(numTriangles / 2);
	shader = ShaderList::getInstance()->getComputeShader(RendEnum::PLANAR_SURFACE_TOPOLOGY);

	shader->bindBuffers(std::vector<GLuint> { modelBufferID, meshBufferID });
	shader->use();
	shader->setUniform("gridSize", uvec2(_tilingH, _tilingV));
	shader->setUniform("numFaces", numTriangles);
	shader->execute(numGroups, 1, 1, ComputeShader::getMaxGroupSize(), 1, 1);

	VertexGPUData* modelData	= shader->readData(modelBufferID, VertexGPUData());
	modelComp->_geometry		= std::move(std::vector<VertexGPUData>(modelData, modelData + numVertices));

	FaceGPUData* faceData		= shader->readData(meshBufferID, FaceGPUData());					// Fully computed after second phase
	modelComp->_topology		= std::move(std::vector<FaceGPUData>(faceData, faceData + numTriangles));

	GLuint* meshData			= shader->readData(rawMeshBufferID, GLuint());
	modelComp->_triangleMesh	= std::move(std::vector<GLuint>(meshData, meshData + numIndices));

	glDeleteBuffers(1, &modelBufferID);
	glDeleteBuffers(1, &meshBufferID);
	glDeleteBuffers(1, &rawMeshBufferID);

	this->generatePointCloud();
	this->generateWireframe();
}

void PlanarSurface::generateWireframe()
{
	// Horizontal lines (width)
	for (unsigned int i = 0; i <= _tilingH; ++i)
	{
		for (unsigned int j = 0; j <= _tilingV; ++j)
		{
			_modelComp[0]->_wireframe.push_back(i * (_tilingH + 1) + j);
		}

		_modelComp[0]->_wireframe.push_back(RESTART_PRIMITIVE_INDEX);
	}

	// Vertical lines (depth)
	for (unsigned int i = 0; i <= _tilingH; ++i)
	{
		for (unsigned int j = 0; j <= _tilingV; ++j)
		{
			_modelComp[0]->_wireframe.push_back(j * (_tilingH + 1) + i);
		}

		_modelComp[0]->_wireframe.push_back(RESTART_PRIMITIVE_INDEX);
	}
}