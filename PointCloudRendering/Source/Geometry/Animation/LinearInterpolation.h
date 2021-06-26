#pragma once

#include "Geometry/Animation/Interpolation.h"
#include "Graphics/Core/ShaderProgram.h"
#include "Graphics/Core/VAO.h"

/**
*	@file LinearInterpolation.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 13/02/2020
*/

/**
*	@brief Wrapping of a raw interpolation shape.
*/
class LinearInterpolation: public Interpolation
{
protected:
	unsigned			_currentIndex;				//!< Current index in vector _parametricPoint (allows us to search in a few indices instead of the whole vector)
	std::vector<float>	_parametricPoint;			//!< Value of parametric t for each waypoint

protected:
	/**
	*	@brief Computes the parametric value for each waypoint.
	*/
	void buildParametricFlow();

public:
	/**
	*	@brief Constructor.
	*	@param waypoints Control points.
	*/
	LinearInterpolation(std::vector<vec4>& waypoints);

	/**
	*	@brief Destructor.
	*/
	virtual ~LinearInterpolation();

	/**
	*	@return Point in a bezier curve for parametric t value.
	*/
	virtual vec4 getPosition(float t, bool& finished);
};

