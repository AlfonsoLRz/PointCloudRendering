#pragma once

/**
*	@file Edge3D.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 08/05/2019
*/

/**
*	@brief Base class for any entity described by a finite or infinite line.
*/
class Edge3D
{
protected:
	vec3	_orig, _dest;							//!< Origin and destination of line (if exists)
	vec3	_direction;								//!< Direction computed from origin and destination

protected:
	/**
	*	@return True if t is in the interval we expect from a certain type of line.
	*/
	virtual bool isParametricTValid(const float t) const = 0;

public:
	/**
	*	@brief Default constructor.
	*/
	Edge3D();

	/**
	*	@brief Constructor.
	*/
	Edge3D(const vec3& orig, const vec3& dest);

	/**
	*	@brief Copy constructor.
	*/
	Edge3D(const Edge3D& edge);

	/**
	*	@brief Destructor.
	*/
	virtual ~Edge3D();

	/**
	*	@return Destination point.
	*/
	vec3 getDest() const { return _dest; }

	/**
	*	@return Edge direction.
	*/
	vec3 getDirection() const { return _direction; }

	/**
	*	@return Initial point.
	*/
	vec3 getOrigin() const { return _orig; }

	/**
	*	@return Point computed from the parametric equation and the value t. Note: Point could be not valid 
	*			if t is not in the valid range for a certain child class.
	*/
	virtual vec3 getPoint(const float t) const;

	/**
	*	@brief Assignment operator overriding.
	*/
	virtual Edge3D& operator=(const Edge3D& edge);

	/**
	*	@brief Modifies the final point.
	*/
	void setDest(const vec3& dest);

	/**
	*	@brief Modifies the edge direction.
	*/
	void setDirection(const vec3& direction);

	/**
	*	@brief Modifies the edge starting point.
	*/
	void setOrigin(const vec3& orig);
};

