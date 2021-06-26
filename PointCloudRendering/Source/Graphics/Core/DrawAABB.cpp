#include "stdafx.h"
#include "DrawAABB.h"

#include "Geometry/3D/TriangleMesh.h"
#include "Graphics/Core/OpenGLUtilities.h"

/// [Public methods]

DrawAABB::DrawAABB(AABB* aabb, const mat4& modelMatrix):
	Model3D(modelMatrix), _aabb(aabb), _vao(Primitives::getCubeVAO())
{
	// Compute model matrix: default cube is 1 x 1 x 1, centered at (0, 0, 0)
	_modelMatrix = _modelMatrix * glm::translate(mat4(1.0f), _aabb->center()) * glm::scale(mat4(1.0f), _aabb->extent() * 2.0f);
}

DrawAABB::~DrawAABB()
{
}

void DrawAABB::drawAsLines(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix)
{
	if (_vao)
	{
		this->setShaderUniforms(shader, shaderType, matrix);

		_vao->drawObject(RendEnum::IBO_WIREFRAME, GL_LINES, 36);
	}
}

void DrawAABB::drawAsPoints(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix)
{
	if (_vao)
	{
		this->setShaderUniforms(shader, shaderType, matrix);

		_vao->drawObject(RendEnum::IBO_POINT_CLOUD, GL_POINTS, 8);
	}
}

void DrawAABB::drawAsTriangles(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix)
{
	if (_vao)
	{
		this->setShaderUniforms(shader, shaderType, matrix);

		_vao->drawObject(RendEnum::IBO_TRIANGLE_MESH, GL_TRIANGLES, 48);
	}
}

void DrawAABB::drawAsTriangles4Fog(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix)
{
	this->drawAsTriangles(shader, shaderType, matrix);
}

void DrawAABB::drawAsTriangles4Shadows(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix)
{
	this->drawAsTriangles(shader, shaderType, matrix);
}

void DrawAABB::drawCapturedPoints(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix)
{
}

bool DrawAABB::load(const mat4& modelMatrix)
{
	this->_loaded = true;

	return true;				
}
