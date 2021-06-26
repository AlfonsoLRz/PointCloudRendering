#pragma once

#include "Geometry/3D/AABB.h"
#include "Geometry/3D/TriangleMesh.h"
#include "Graphics/Core/Model3D.h"
#include "Graphics/Core/VAO.h"

class TriangleMesh;

/**
*	@file DrawAABB.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 08/16/2019
*/

/**
*	@brief AABB painter.
*/
class DrawAABB: public Model3D
{
protected:
	AABB*	_aabb;					//!< Axis aligned bounding box to be queried about its geometry
	VAO*	_vao;					//!< Geometry and topology in GPU

public:
	/**
	*	@brief Constructor.
	*	@param aabb Axis aligned bounding box.
	*	@param modelMatrix First model transformation.
	*/
	DrawAABB(AABB* aabb, const mat4& modelMatrix = mat4(1.0f));

	/**
	*	@brief Destructor.
	*/
	~DrawAABB();

	/**
	*	@brief Modifies model vertices from local space to world space.
	*/
	virtual void computeWorldPosition(mat4& modelMatrix) {}

	/**
	*	@brief Renders the AABB as a set of lines.
	*	@param shader Shader which will draw the model.
	*	@param shaderType Index of shader at the list, so we can identify what type of uniform variables must be declared.
	*	@param matrix Vector of matrices which can be applied in the rendering process.
	*/
	virtual void drawAsLines(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix);

	/**
	*	@brief Renders the AABB as a set of points.
	*	@param shader Shader which will draw the model.
	*	@param shaderType Index of shader at the list, so we can identify what type of uniform variables must be declared.
	*	@param matrix Vector of matrices which can be applied in the rendering process.
	*/
	virtual void drawAsPoints(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix);

	/**
	*	@brief Renders the AABB as a set of triangles.
	*	@param shader Shader which will draw the model.
	*	@param shaderType Index of shader at the list, so we can identify what type of uniform variables must be declared.
	*	@param matrix Vector of matrices which can be applied in the rendering process.
	*/
	virtual void drawAsTriangles(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix);

	/**
	*	@brief Renders the AABB as a set of triangles with no textures as we only want to color fragments taking into account its depth.
	*	@param shader Shader which will draw the model.
	*	@param shaderType Index of shader at the list, so we can identify what type of uniform variables must be declared.
	*	@param matrix Vector of matrices which can be applied in the rendering process.
	*/
	virtual void drawAsTriangles4Fog(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix);

	/**
	*	@brief Renders the AABB as a set of triangles with no textures (except disp. mapping) as we only want to retrieve the depth.
	*	@param shader Shader which will draw the model.
	*	@param shaderType Index of shader at the list, so we can identify what type of uniform variables must be declared.
	*	@param matrix Vector of matrices which can be applied in the rendering process.
	*/
	virtual void drawAsTriangles4Shadows(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix);

	/**
	*	@brief Renders the points captured by a sensor (coloured).
	*	@param shader Shader which will draw the model.
	*	@param shaderType Index of shader at the list, so we can identify what type of uniform variables must be declared.
	*	@param matrix Vector of matrices which can be applied in the rendering process.
	*/
	virtual void drawCapturedPoints(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix);

	/**
	*	@brief Computes the AABB data and sends it to GPU.
	*	@return Success of operation.
	*/
	virtual bool load(const mat4& modelMatrix = mat4(1.0f));
};

