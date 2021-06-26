#pragma once

#include "Geometry/3D/Edge3D.h"

/**
*	@file Line3D.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 08/05/2019
*/

/**
*	@brief Infinite edge (line) with no origin or destination, even tho we need two points to compute direction.
*/
class Line3D: public Edge3D
{
protected:
	/**
	*	@return True if t is in the interval we expect from a certain type of line.
	*/
	virtual bool isParametricTValid(const float t) const;

public:
	/**
	*	@brief Default constructor.
	*/
	Line3D();

	/**
	*	@brief Constructor.
	*/
	Line3D(const vec3& orig, const vec3& dest);

	/**
	*	@brief Copy constructor.
	*/
	Line3D(const Line3D& line);

	/**
	*	@brief Destructor.
	*/
	virtual ~Line3D();

	/**
	*	@brief Returns the distance between the line and the point.
	*/
	float distance(const vec3& point) const;

	/**
	*	@brief Distance between two lines.
	*/
	float distance(Line3D& line) const;

	/**
	*	@brief Returns the normal vector to this line that goes through p.
	*/
	Line3D normalToPoint(const vec3& p) const;

	/**
	*	@brief Assignment operator overriding.
	*/
	virtual Line3D& operator=(const Line3D& line);

	/**
	*	@return True if both lines are parallel.
	*/
	bool parallel(Line3D& line) const;

	/**
	*	@return True if both lines are perpendicular.
	*/
	bool perpendicular(Line3D& line) const;
};

