#pragma once

#include "stdafx.h"

/**
*	@file OpenGLUtilities.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 08/03/2019
*/

/**
*	@brief Several operations which can be useful for different entities.
*/
namespace ColorUtilities
{
	namespace Constraints		// Private namespace
	{
		const float PHI = 0.618033988749895f;
	};

	/**
	*	@brief Assigns a hue value for every id. The purpose is to obtain colors which are properly distantiated.
	*/
	float getHueValue(const unsigned id);

	/**
	*	@return RGB color corresponding to the initial HSV color.
	*/
	vec3 HSVtoRGB(float h, float s, float v);

	/**
	*	@return HSV color corresponding to the initial RGB color.
	*/
	vec3 RGBtoHSV(float r, float g, float b);
};

inline float ColorUtilities::getHueValue(const unsigned id)
{
	const float h = id * ColorUtilities::Constraints::PHI;

	return fmod(h, 1.0f);
}

inline vec3 ColorUtilities::HSVtoRGB(float h, float s, float v)
{
	const int h_i = (int)(h * 6.0f);
	const float f = h * 6.0f - h_i;
	const float p = v * (1.0f - s);
	const float q = v * (1.0f - f * s);
	const float t = v * (1.0f - (1.0f - f) * s);

	switch (h_i)
	{
	case 0:
		return vec3(v, t, p);
	case 1:
		return vec3(q, v, p);
	case 2:
		return vec3(p, v, t);
	case 3:
		return vec3(p, q, v);
	case 4:
		return vec3(t, p, v);
	case 5:
		return vec3(v, p, q);
	}

	return vec3(.0f);
}

inline vec3 ColorUtilities::RGBtoHSV(float r, float g, float b)
{
	float min = std::min(std::min(r, g), b), max = std::max(std::max(r, g), b);
	float delta = max - min;
	vec3 hsv;
	
	hsv.z = max;

	if (delta < glm::epsilon<float>())
	{
		return vec3(.0f, .0f, max);
	}

	if (max > 0.0)								// If Max is == 0, this divide would cause a crash
	{						
		hsv.y = (delta / max);                  // s
	}
	else 
	{
		return vec3(NAN, .0f, max);
	}

	if (r >= max)
	{
		hsv.x = (g - b) / delta;				// Between yellow & magenta
	}
	else
	{
		if (g >= max)
			hsv.x = 2.0 + (b - r) / delta;		// Between cyan & yellow
		else
			hsv.x = 4.0 + (r - g) / delta;		// Between magenta & cyan
	}

	hsv.x *= 60.0;                              // To degrees

	if (hsv.x < 0.0f)
	{
		hsv.x += 360.0;
	}

	hsv.x /= 360.0f;							// [0, 1]

	return hsv;
}



