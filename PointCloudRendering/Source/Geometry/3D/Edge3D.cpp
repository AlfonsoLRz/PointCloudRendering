#include "stdafx.h"
#include "Edge3D.h"

/// [Public methods]

Edge3D::Edge3D() :
	_orig(0.0f), _dest(0.0f), _direction(0.0f)
{
}

Edge3D::Edge3D(const vec3& orig, const vec3& dest) : 
	_orig(orig), _dest(dest), _direction(_dest - _orig)
{
}

Edge3D::Edge3D(const Edge3D& edge) :
	_orig(edge._orig), _dest(edge._dest), _direction(edge._direction)
{
}

Edge3D::~Edge3D()
{
}

vec3 Edge3D::getPoint(const float t) const
{
	if (!this->isParametricTValid(t))
	{
		return vec3();
	}

	return _orig + _direction * t;
}

Edge3D& Edge3D::operator=(const Edge3D& edge)
{
	_orig		= edge._orig;
	_dest		= edge._dest;
	_direction	= edge._direction;

	return *this;
}

void Edge3D::setDest(const vec3& dest)
{
	_dest		= dest;
	_direction	= _dest - _orig;
}

void Edge3D::setDirection(const vec3& direction)
{
	_direction	= direction;
	_dest		= _orig + _direction;
}

void Edge3D::setOrigin(const vec3& orig)
{
	_orig		= orig;
	_direction	= _dest - _orig;
}
