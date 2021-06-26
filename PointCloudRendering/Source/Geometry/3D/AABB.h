#pragma once

/**
*	@file AABB.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 08/05/2019
*/

/**
*	@brief Axis-aligned bounding box.
*/
class AABB
{
protected:
	vec3	_max, _min;						//!< Minimum and maximum points (max and min value at X, Y and Z axes)

public:
	/**
	*	@brief Constructor.
	*/
	AABB(const vec3& min = vec3(INFINITY), const vec3& max = vec3(-INFINITY));

	/**
	*	@brief Copy constructor.
	*/
	AABB(const AABB& aabb);

	/**
	*	@brief Destructor.
	*/
	virtual ~AABB();

	/**
	*	@brief Assignment operator overriding.
	*/
	AABB& operator=(const AABB& aabb);

	/**
	*	@return Middle point of cube. 
	*/
	vec3 center() const { return (_max + _min) / 2.0f; }

	/**
	*	@return Vector from center to maximum point. Maximum point - center.
	*/
	vec3 extent() const { return _max - center(); }

	/**
	*	@return Maximum point.
	*/
	vec3 max() const { return _max; }

	/**
	*	@return Minimum point.
	*/
	vec3 min() const { return _min; }

	/**
	*	@return AABB size for each edge.
	*/
	vec3 size() const { return _max - _min; }

	/**
	*	@brief Divides any axis as many times as edgeDivisions, creating a set of smaller bounding boxes.
	*/
	std::vector<AABB> split(const unsigned edgeDivisions) const;

	/**
	*	@brief Updates the boundaries with a new axis aligned bounding box.
	*/
	void update(const AABB& aabb);

	/**
	*	@brief Updates the maximum and minimum corners if point is outside the bounding box.
	*/
	void update(const vec3& point);
};

