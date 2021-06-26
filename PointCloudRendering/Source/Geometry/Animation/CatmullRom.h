#pragma once

#include "Geometry/Animation/Interpolation.h"

/**
*	@file CatmullRom.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 22/02/2021
*/

/**
*	@brief Wrapper of Inigo Quilez's Catmull Rom definition.
*/
class CatmullRom : public Interpolation
{
protected:
	static const float COEFFICIENTS[16];

protected:
	std::vector<float> _timeKey;			//!<

public:
	/**
	*	@brief Constructor.
	*	@param waypoints Main points which belong to the interpolation shape.
	*/
	CatmullRom(std::vector<vec4>& waypoints);

	/**
	*	@brief Destructor.
	*/
	virtual ~CatmullRom();

	/**
	*	@brief Computes a Catmull Rom spline with 'resolution' splits.
	*/
	void getPath(std::vector<vec4>& path, const unsigned resolution);

	/**
	*	@return Point of a Catmull Rom curve for parametric value t.
	*/
	virtual vec4 getPosition(float t, bool& finished);

	// Setters

	/**
	*	@brief Modifies time key for our waypoints.
	*/
	void setTimeKey(const std::vector<float>& timeKey) { _timeKey = timeKey; }

	/**
	*	@brief Modifies spline path. 
	*/
	void setWaypoints(const std::vector<vec4>& path) { _waypoints = path; }
};

