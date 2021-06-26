#pragma once

#include "Geometry/3D/Edge3D.h"

/**
*	@file Segment3D.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 08/06/2019
*/

/**
*	@brief Edge with origin and destination.
*/
class Segment3D: public Edge3D
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
	Segment3D();

	/**
	*	@brief Constructor.
	*/
	Segment3D(const vec3& orig, const vec3& dest);

	/**
	*	@brief Copy constructor.
	*/
	Segment3D(const Segment3D& segment);

	/**
	*	@brief Destructor.
	*/
	virtual ~Segment3D();

	/**
	*	@brief Assignment operator overriding.
	*/
	virtual Segment3D& operator=(const Segment3D& segment);
};

