#pragma once

#include "stdafx.h"
#include "Graphics/Core/Model3D.h"
#include "Graphics/Core/VAO.h"

/**
*	@file OpenGLUtilities.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 08/03/2019
*/

/**
*	@brief Several operations which can be useful for different entities.
*/
namespace OpenGLUtilities
{
};

class Primitives
{
protected:
	static std::unique_ptr<VAO> QUAD_VAO;
	static std::unique_ptr<VAO> CUBE_VAO;

public:
	/**
	*	@return Eight points which represents the given cube.
	*/
	static std::vector<vec4> getCubePoints(const vec3& minValues, const vec3& maxValues);

	/**
	*	@return Vector of indices which represents the cube wireframe shape.
	*/
	static std::vector<GLuint> getCubeWireframeIndices(const unsigned startIndex);

	/**
	*	@return VAO with geometry and topology of a cube.
	*/
	static VAO* getCubeVAO();

	/**
	*	@param nodes Array of cubes to be specified.
	*	@param nNodes Number of cubes to be taken into account.
	*	@return VAO with geometry and topology of a set of cubes.
	*/
	static VAO* getCubesVAO(std::vector<Model3D::BVHCluster>& nodes, unsigned nNodes);

	/**
	*	@return VAO with geometry and topology of a quad
	*/
	static VAO* getNormalizedMeshVAO(unsigned numDivs, unsigned& numIndices);

	/**
	*	@return Model component with geometry and topology of a quad.
	*/
	static Model3D::ModelComponent* getQuadModelComp();

	/**
	*	@return VAO with geometry and topology of a quad.
	*/
	static VAO* getQuadVAO();

	/**
	*	@return Model component with a flexible resolution defined by sectors and stacks.
	*/
	static Model3D::ModelComponent* getSphereModelComp(const unsigned sectors, const unsigned stacks, const float radius = 1.0f);
};



