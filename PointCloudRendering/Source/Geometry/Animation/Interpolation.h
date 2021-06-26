#pragma once

#include "Graphics/Core/Model3D.h"
#include "Graphics/Core/ShaderProgram.h"
#include "Graphics/Core/VAO.h"

/**
*	@file Interpolation.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 13/02/2020
*/

/**
*	@brief Abstract class which wraps methods and attributes for any interpolation.
*/
class Interpolation
{
protected:
	std::vector<vec4>	_waypoints;

public:
	/**
	*	@brief Constructor.
	*	@param waypoints Main point which conform interpolation shape.
	*/
	Interpolation(std::vector<vec4>& waypoints);

	/**
	*	@brief Destructor.
	*/
	virtual ~Interpolation();

	/**
	*	@return Pointer of original path.
	*/
	std::vector<vec4>* getWaypoints() { return &_waypoints; }

	/**
	*	@return Position of interpolation function for a parametric value t.
	*/
	virtual vec4 getPosition(float t, bool& finished) = 0;

	// Static methods
	
	/**
	*	@return Absolute distance of points, from start to end (not a curve).
	*/
	static float getLineLength(std::vector<vec4>& points);
};

