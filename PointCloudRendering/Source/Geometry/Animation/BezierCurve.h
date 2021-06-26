#pragma once

#include "Geometry/Animation/Interpolation.h"

/**
*	@file BezierCurve.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es).
*	@date 02/20/2020
*/

/**
*	@brief Wrapper of a Bezier curve.
*	@author Alfonso López Ruiz.
*/
class BezierCurve : public Interpolation
{
private:
	/**
	*	@return Interpolated point from a quadratic bezier curve. 
	*/
	vec4 getPoint(const unsigned index1, const unsigned index2, const unsigned index3, const unsigned index4, const float t);

public:
	/**
	*	@brief Constructor.
	*	@param waypoints Main points which conform interpolation shape.
	*/
	BezierCurve(std::vector<vec4>& waypoints);

	/**
	*	@brief Destructor.
	*/
	virtual ~BezierCurve();

	/**
	*	@return Point of a bezier curve for parametric value t.
	*/
	virtual vec4 getPosition(float t, bool& finished);
};

