#include "stdafx.h"
#include "Triangle3D.h"

#include "Geometry/3D/Plane.h"
#include "Geometry/General/BasicOperations.h"

/// [Public methods]

Triangle3D::Triangle3D()
{
}

Triangle3D::Triangle3D(const vec3& va, const vec3& vb, const vec3& vc) :
	_a(va), _b(vb), _c(vc)
{
}

Triangle3D::Triangle3D(const Triangle3D& triangle) :
	_a(triangle._a), _b(triangle._b), _c(triangle._c)
{
}

Triangle3D::~Triangle3D()
{
}

float Triangle3D::area() const
{
	const vec3 u = _b - _a, v = _c - _a;

	return glm::length(glm::cross(u, v)) / 2.0f;
}

Triangle3D::PointPosition Triangle3D::classify(const vec3& point)
{
	const vec3 v = point - _a;
	const float module = glm::length(v);

	if (BasicOperations::equal(module, 0.0f))
	{
		return PointPosition::COPLANAR;
	}

	const float d = glm::dot(glm::normalize(v), normal());

	if (d > glm::epsilon<float>())
	{
		return PointPosition::POSITIVE;
	}
	else if (d < glm::epsilon<float>())
	{
		return PointPosition::NEGATIVE;
	}

	return PointPosition::COPLANAR;
}

float Triangle3D::distance(const vec3& point) const
{
	Plane plane(_a, _b, _c, true);

	return plane.distance(point);
}

float Triangle3D::getAlpha(Edge3D& edge) const
{
	const vec3 normal	= this->normal();
	const vec3 u		= edge.getDest() - edge.getOrigin();
	const float num		= normal.x * u.x + normal.y * u.y + normal.z * u.z;
	const float den		= std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z) * std::sqrt(u.x * u.x + u.y * u.y + u.z * u.z);

	return std::asin(num / den);
}

float Triangle3D::getAlpha(Triangle3D& triangle) const
{
	const vec3 n_1 = this->normal(), n_2 = triangle.normal();
	const float dotN = glm::dot(n_1, n_2);

	return dotN / (glm::length(n_1) * glm::length(n_2));
}

vec3 Triangle3D::getCenterOfMass() const
{
	return (_a + _b + _c) * 1.0f / 3.0f;
}

vec3 Triangle3D::normal() const
{
	const vec3 u = _b - _a, v = _c - _a;
	const vec3 n = glm::cross(u, v);

	return glm::normalize(n);
}

Triangle3D& Triangle3D::operator=(const Triangle3D& triangle)
{
	_a = triangle._a;
	_b = triangle._b;
	_c = triangle._c;

	return *this;
}

void Triangle3D::set(const vec3& va, const vec3& vb, const vec3& vc)
{
	_a = va;
	_b = vb;
	_c = vc;
}
