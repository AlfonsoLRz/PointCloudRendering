#pragma once

#include "Geometry/3D/Edge3D.h"

/**
*	@file Triangle3D.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 08/05/2019
*/

/**
*	@brief Triangle in 3D.
*/
class Triangle3D
{
public:
	enum PointPosition
	{
		POSITIVE, NEGATIVE, COPLANAR
	};

protected:
	vec3 _a, _b, _c;						//!< Points in counterclockwise direction in an ideal situation (no guaranteed as the user is who specifies these values)

public:
	/**
	*	@brief Default constructor.
	*/
	Triangle3D();

	/**
	*	@brief Constructor from three points.
	*/
	Triangle3D(const vec3& va, const vec3& vb, const vec3& vc);

	/**
	*	@brief Copy constructor.
	*/
	Triangle3D(const Triangle3D& triangle);

	/**
	*	@brief Destructor.
	*/
	virtual ~Triangle3D();

	/**
	*	@return Area of triangle.
	*/
	float area() const;

	/**
	*	@return Position of a point respect to the triangle.
	*/
	PointPosition classify(const vec3& point);

	/**
	*	@return Distance from triangle to point.
	*/
	float distance(const vec3& point) const;

	/**
	*	@return Angle between the triangle and the line.
	*/
	float getAlpha(Edge3D& edge) const;

	/**
	*	@return Angle between two triangles (which are actually planes).
	*/
	float getAlpha(Triangle3D& triangle) const;

	/**
	*	@return Center of mass of triangle.
	*/
	vec3 getCenterOfMass() const;

	/**
	*	@return Selected point from the three that forms the triangle.
	*/
	vec3 getPoint(int i)  const { return (i == 0 ? _a : (i == 1 ? _b : _c)); }

	/**
	*	@brief Returns the first point.
	*/
	vec3 getP1() const { return _a; }

	/**
	*	@brief Returns the second point.
	*/
	vec3 getP2() const { return _b; }

	/**
	*	@brief Returns the third point.
	*/
	vec3 getP3() const { return _c; }

	/**
	*	@return Normal vector of triangle.
	*/
	vec3 normal() const;

	/**
	*	@brief Assignment operator overriding.
	*/
	virtual Triangle3D& operator=(const Triangle3D& triangle);

	/**
	*	@brief Modifies the first point.
	*/
	void setA(const vec3& p1) { _a = p1; }

	/**
	*	@brief Modifies the second point.
	*/
	void setB(const vec3& p2) { _b = p2; }

	/**
	*	@brief Modifies the third point.
	*/
	void setC(const vec3& p3) { _c = p3; }

	/**
	*	@brief Modifies all the points.
	*/
	void set(const vec3& va, const vec3& vb, const vec3& vc);
};

