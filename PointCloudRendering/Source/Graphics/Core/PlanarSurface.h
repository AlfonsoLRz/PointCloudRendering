#pragma once

#include "Geometry/3D/TriangleMesh.h"
#include "Graphics/Core/Model3D.h"

/**
*	@file PlanarSurface.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/23/2019
*/

/**
*	@brief 3D model of a rectangle or plane with a number of subdivisions which can be specified.
*/
class PlanarSurface: public Model3D
{
protected:
	// [PlanarSurface properties]
	float		_width, _depth;										
	float		_maxTextValH, _maxTextValV;									
	uint16_t	_tilingH, _tilingV;

protected:
	/**
	*	@brief Computes each vertex attributes.
	*	@param modelMatrix Transformation which must be applied to a plane model to render it.
	*/
	void generateGeometryTopology(const mat4& modelMatrix);

	/**
	*	@brief
	*/
	virtual void generateWireframe();

public:
	/**
	*	@brief Constructor of a plane of any length and subdivisions. 
	*	@param width PlanarSurface with.
	*	@param height PlanarSurface height.
	*	@param tilingH Number of horizontal subdivisions.
	*	@param tilingV Number of vertical subdivisions.
	*	@param maxTextValH Maximum texture coordinate at the X axis.
	*	@param maxTextValV Maximum texture coordinate at the Y axis.
	*	@param modelMatrix Transformation which must be applied to a plane model to render it.
	*/
	PlanarSurface(const float width = 1.0f, const float depth = 1.0f, const uint16_t tilingH = 1, const uint16_t tilingV = 1, 
				  const float maxTextValH = 1.0f, const float maxTextValV = 1.0f, const mat4& modelMatrix = mat4(1.0f));

	/**
	*	@brief Deleted copy constructor.
	*/
	PlanarSurface(const PlanarSurface& plane) = delete;

	/**
	*	@brief Destructor.
	*/
	~PlanarSurface();

	/**
	*	@brief Renders the model as a set of lines.
	*	@param shader Shader which will draw the model.
	*	@param shaderType Index of shader at the list, so we can identify what type of uniform variables must be declared.
	*	@param matrix Vector of matrices which can be applied in the rendering process.
	*/
	virtual void drawAsLines(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix);

	/**
	*	@brief Renders the model as a set of triangles.
	*	@param shader Shader which will draw the model.
	*	@param shaderType Index of shader at the list, so we can identify what type of uniform variables must be declared.
	*	@param matrix Vector of matrices which can be applied in the rendering process.
	*/
	virtual void drawAsTriangles(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix);

	/**
	*	@brief Renders the model as a set of triangles with no texture as we only want to retrieve the depth.
	*	@param shader Shader which will draw the model.
	*	@param shaderType Index of shader at the list, so we can identify what type of uniform variables must be declared.
	*	@param matrix Vector of matrices which can be applied in the rendering process.
	*/
	virtual void drawAsTriangles4Shadows(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix);

	/**
	*	@return Number of cells in the terrain (it is related to the number of triangles). Horizontal, vertical.
	*/
	vec2 getNumTiles() { return vec2(_tilingH, _tilingV); }

	/**
	*	@return Size of map in width, depth format.
	*/
	vec2 getSize() { return vec2(_width, _depth); }

	/**
	*	@brief Loads the plane data into GPU.
	*	@return Success of operation.
	*/
	virtual bool load(const mat4& modelMatrix = mat4(1.0f));

	/**
	*	@brief Deleted assignment operator overriding.
	*	@param plane PlanarSurface from where we need to copy attributes.
	*/
	PlanarSurface& operator=(const PlanarSurface& plane) = delete;
};

