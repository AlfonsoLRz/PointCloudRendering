#include "stdafx.h"
#include "CatmullRom.h"

/// [Static attributes]

const float CatmullRom::COEFFICIENTS[16] = { -1, 2, -1, 0, 3, -5, 0, 2, -3, 4, 1, 0, 1, -1, 0, 0 };

/// [Public methods]

CatmullRom::CatmullRom(std::vector<vec4>& waypoints) : Interpolation(waypoints)
{
}

CatmullRom::~CatmullRom()
{
}

vec4 CatmullRom::getPosition(float t, bool& finished)
{
    if (t < glm::epsilon<float>() && !_waypoints.empty()) return _waypoints[0];
    else if (t >= 1.0f && !_waypoints.empty()) return _waypoints[_waypoints.size() - 1];
	
    const unsigned size = std::min(_timeKey.size(), _waypoints.size());
    int k = 0; while (_timeKey[k] < t) k++;                                             // Find key
    const float h = (t - _timeKey[glm::clamp(k - 1, 0, k)]) / (_timeKey[k] - _timeKey[glm::clamp(k - 1, 0, k)]);            // Interpolant
    vec4 result(.0f);

    for (int i = 0; i < 4; i++)
    {
        int kn = k + i - 2;

    	if (kn < 0) kn = 0;
    	else if (kn > (size - 1)) kn = size - 1;
    	
        const float* coefficient = COEFFICIENTS + 4 * i;
        const float b = 0.5f * (((coefficient[0] * h + coefficient[1]) * h + coefficient[2]) * h + coefficient[3]);
    	
        result += b * _waypoints[kn];
    }

    return result;
}

void CatmullRom::getPath(std::vector<vec4>& path, const unsigned resolution)
{
    const float tIncrement = 1.0f / resolution;
    float t = .0f;
    bool finished;

	while (t <= 1.0f)
	{
        path.push_back(this->getPosition(t, finished));
        t += tIncrement;
	}
}
