#include "stdafx.h"
#include "PointCloud3D.h"

/// [Public methods]

PointCloud3D::PointCloud3D()
{
}

PointCloud3D::PointCloud3D(int numPoints, const AABB& aabb)
{
	const vec3 size = aabb.size();
	const vec3 min	= aabb.min();

	while (numPoints > 0)
	{
		const float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (size.x * 2.0f))) + min.x;
		const float y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (size.y * 2.0f))) + min.y;
		const float z = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (size.z * 2.0f))) + min.z;
		this->push_back(vec3(x, y, z));

		--numPoints;
	}
}

PointCloud3D::PointCloud3D(int numPoints, float radius)
{
	while (numPoints > 0)
	{
		const float theta	= static_cast <float> (rand()) / (static_cast <float> (RAND_MAX)) * 2.0f * glm::pi<float>();
		const float phi		= std::acos(1.0f - 2.0f * static_cast <float> (rand()) / (static_cast <float> (RAND_MAX)));
		const double x		= std::sin(phi) * std::cos(theta);
		const double y		= std::sin(phi) * std::sin(theta);
		const double z		= std::cos(phi);
		const float r		= radius * std::sqrt(static_cast <float> (rand()) / (static_cast <float> (RAND_MAX)));

		this->push_back(vec3(r * x, r * y, r * z));

		--numPoints;
	}
}

PointCloud3D::PointCloud3D(const PointCloud3D& pointCloud) :
	_points(pointCloud._points), _aabb(pointCloud._aabb)
{
}

PointCloud3D::~PointCloud3D()
{
}

vec3 PointCloud3D::getPoint(const int index) const
{
	if (index >= _points.size()) return vec3();

	return _points[index];
}

void PointCloud3D::joinPointCloud(PointCloud3D* pointCloud)
{
	if (pointCloud && pointCloud->_points.size())
	{
		for (vec4& point : pointCloud->_points)
		{
			this->push_back(point);
		}
	}
}

PointCloud3D& PointCloud3D::operator=(const PointCloud3D& pointCloud)
{
	if (this != &pointCloud)
	{
		_points = pointCloud._points;
		_aabb	= pointCloud._aabb;
	}

	return *this;
}

void PointCloud3D::push_back(const vec3& point)
{
	_points.push_back(vec4(point, 1.0f));
	_aabb.update(point);
}
