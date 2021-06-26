#pragma once

#include "Geometry/3D/AABB.h"

/**
*	@file PointCloud3D.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 08/31/2019
*/

/**
*	@brief Wrapper for a set of point in a 3D space.
*/
class PointCloud3D
{
	std::vector<vec4>	_points;					//!< Point cloud
	AABB				_aabb;						//!< Boundaries

public:
	/**
	*	@brief Default constructor.
	*/
	PointCloud3D();

	/**
	*	@brief Constructor of a randomized square-shaped point cloud.
	*	@param aabb Point cloud boundaries.
	*/
	PointCloud3D(int numPoints, const AABB& aabb);

	/**
	*	@brief Alternative constructor with an uniform sphere distribution.
	*/
	PointCloud3D(int numPoints, float radius);

	/**
	*	@brief Copy constructor.
	*/
	PointCloud3D(const PointCloud3D& pointCloud);

	/**
	*	@brief Destructor.
	*/
	virtual ~PointCloud3D();

	/**
	*	@brief Removes all the points.
	*/
	void clear() { _points.clear(); }

	/**
	*	@brief 
	*/
	AABB getAABB() { return _aabb; }

	/**
	*	@return Point in index position in array.
	*/
	vec3 getPoint(const int index) const;

	/**
	*	@brief Returns all the points in the cloud.
	*/
	std::vector<vec4>* getPoints() { return &_points; }

	/**
	*	@brief Modifies this point cloud as it now contains the points included in the new point cloud.
	*/
	void joinPointCloud(PointCloud3D* pointCloud);

	/**
	*	@brief Assigment operator.
	*/
	virtual PointCloud3D& operator=(const PointCloud3D& pointCloud);

	/**
	*	@brief Adds a new point to the cloud.
	*/
	void push_back(const vec3& point);

	/**
	*	@brief Number of points that this cloud contains.
	*/
	size_t size() const { return _points.size(); }
};

