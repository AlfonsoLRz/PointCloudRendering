#include "stdafx.h"
#include "Plane.h"

#include "Geometry/General/BasicOperations.h"

/// [Public methods]

Plane::Plane(const vec3& point, const vec3& u, const vec3& v, bool arePoints)
{
	if (arePoints)
	{
		_a = point; _b = u; _c = v;
		_u = _b - _a; _v = _c - _a;
	}
	else                          
	{										
		_a = point;								// pi = p + u * lambda + v * mu
		_b = _a + u;
		_c = _a + v;
		_u = u; _v = v;
	}

	_coefficients = computeInterceptFormCoeff();
}

Plane::Plane(const Plane& plane) :
	_a (plane._a), _b(plane._b), _c(plane._c), _u(plane._u), _v(plane._v), _coefficients(plane._coefficients)
{
}

Plane::~Plane()
{
}

bool Plane::coplanar(const vec3& point) const
{
	return BasicOperations::equal(distance(point), 0.0f);
}

float Plane::distance(const vec3& point) const
{
	const vec4 coefficients = interceptFormCoeff();

	return coefficients.x * point.x + coefficients.y * point.y + coefficients.z * point.z + coefficients.w;
}

vec3 Plane::normal() const
{
	const vec4 coefficients = interceptFormCoeff();
	return glm::normalize(vec3(coefficients.x, coefficients.y, coefficients.z));				// A, B and C
}

vec3 Plane::getPoint(const float lambda, const float mu) const
{
	return _a + _u * lambda + _v * mu;
}

Plane& Plane::operator=(const Plane& plane)
{
	_a = plane._a;
	_b = plane._b;
	_c = plane._c;
	_u = plane._u;
	_v = plane._v;
	_coefficients = plane._coefficients;

	return *this;
}

/// [Protected methods]

vec4 Plane::computeInterceptFormCoeff() const
{
	const float a = BasicOperations::determinant2x2(_v.z, _v.y, _u.z, _v.y);
	const float b = BasicOperations::determinant2x2(_v.x, _v.z, _u.x, _u.z);
	const float c = BasicOperations::determinant2x2(_v.y, _v.x, _u.y, _u.x);
	const float d = -(a * _a.x + b * _b.y + c * _a.z);									// Any point from the plane is actually valid

	return vec4(a, b, c, d);
}
