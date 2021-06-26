#pragma once

#include "stdafx.h"

/**
*	@file ImageUtilities.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 08/07/2019
*/

/**
*	@brief Wrapper for image operations.
*/
namespace ImageUtilities
{
	/**
	*	@param image Float array which defines an image of depth 1.
	*	@param width, height Image size.
	*	@param u, v Texture coordinates (horizontal and vertical).
	*	@return Image color for texture coordinates u and v.
	*/
	float bilinearFilter(const float* image, const int width, const int height, const float u, const float v);
};

inline float ImageUtilities::bilinearFilter(const float* image, const int width, const int height, const float u, const float v)
{
	const float ut	= std::fmod(u, 1.0f), vt = std::fmod(v, 1.0f);
	const int xl	= (int)(ut * (width - 1) - 0.5f), xr = (int)(ut * (width - 1) + 0.5f);
	const int yb	= (int)(vt * (height - 1) - 0.5f), yt = (int)(vt * (height - 1) + 0.5f);

	float value = (1.0f - ut) * (1.0f - vt) * image[width * yb + xl] + ut * (1.0f - vt) * image[width * yb + xr];
	value += (1.0f - ut) * vt * image[width * yt + xl] + ut * vt * image[width * yt + xr];

	return value;
}

