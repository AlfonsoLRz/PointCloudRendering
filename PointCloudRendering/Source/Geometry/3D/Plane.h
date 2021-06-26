#pragma once

/**
*	@file Plane.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 08/05/2019
*/

/**
*	@brief Plane described by a planar surface in 3D.
*/
class Plane
{
protected:
	vec3	_a, _b, _c;							//!< Three plane points
	vec3	_u, _v;								//!< Parametric vectors to compute new points from lambda and mu
	vec4	_coefficients;						//!< Ax + By + Cz + D = 0

protected:
	/**
	*	@brief Computes the four coefficients which describes the intercept form equation.
	*/
	vec4 computeInterceptFormCoeff() const;

public:
	/**
	*	@brief Constructor.
	*	@param p p in pi = p + u * lambda + v * mu -> r from the points (R, S, T).
	*	@param u u in pi = p + u * lambda + v * mu -> d from the points (R, S, T).
	*	@param v v in pi = p + u * lambda + v * mu -> t from the points (R, S, T).
	*	@param arePoints If arePoints is false, then params are p + u * lambda + v * mu, otherwise are three points (R, S, T).
	*/
	Plane(const vec3& point, const vec3& u, const vec3& v, bool arePoints);

	/**
	*	@brief Copy constructor.
	*/
	Plane(const Plane& plane);

	/**
	*	@brief Destructor.
	*/
	virtual ~Plane();

	/**
	*	@return True if point is contained in the plane.
	*/
	bool coplanar(const vec3& point) const;

	/**
	*	@return Distance between plane and point.
	*/
	float distance(const vec3& point) const;

	/**
	*	@return Coefficients of a plane equation in intercept form. 
	*/
	vec4 interceptFormCoeff()  const { return _coefficients; }

	/**
	*	@return Plane normal.
	*/
	vec3 normal() const;

	/**
	*	@return Point computed from the parametric function (plane = p + u * lambda + v * mu).
	*/
	vec3 getPoint(const float lambda, const float mu) const;

	/**
	*	@brief Assignment operator overriding.
	*/
	virtual Plane& operator=(const Plane& plane);
};

