#include "stdafx.h"
#include "LinearInterpolation.h"

/// Public methods

LinearInterpolation::LinearInterpolation(std::vector<vec4>& waypoints) :
	Interpolation(waypoints), _currentIndex(1)
{
	this->buildParametricFlow();
}

LinearInterpolation::~LinearInterpolation()
{
}

vec4 LinearInterpolation::getPosition(float t, bool& finished)
{
	if (_currentIndex >= _parametricPoint.size())
	{
		finished = true;
		return vec4();
	}

	while (_parametricPoint[_currentIndex] < t)
	{
		++_currentIndex;

		if (_currentIndex >= _parametricPoint.size())
		{
			finished = true;
			return vec4();
		}
	}

	float weight = (t - _parametricPoint[_currentIndex - 1]) / (_parametricPoint[_currentIndex] - _parametricPoint[_currentIndex - 1]);
	finished = false;

	return weight * _waypoints[_currentIndex] + (1.0f - weight) * _waypoints[_currentIndex - 1];
}

/// Protected methods

void LinearInterpolation::buildParametricFlow()
{
	float totalLength = Interpolation::getLineLength(_waypoints), accumDistance = 0.0f;
	_parametricPoint.clear(); _parametricPoint.push_back(0.0f);

	for (int i = 1; i < (int)_waypoints.size() - 1; ++i)
	{
		accumDistance += glm::distance(_waypoints[i], _waypoints[i - 1]);
		_parametricPoint.push_back(accumDistance / totalLength);
	}

	_parametricPoint.push_back(1.0f);
}
