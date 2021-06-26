#pragma once

#include "stdafx.h"

/**
*	@file BasicOperations.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/10/2019
*/

/**
*	@brief Defines some basic operations for the geometry module.
*/
namespace BasicOperations
{
	/**
	*	@brief 2x2 determinant.
	*/
	float determinant2x2(const float a, const float b, const float c, const float d);

	/**
	*	@brief 3x3 determinant.
	*/
	float determinant3x3(const float a, const float b, const float c, const float d, const float e, const float f, const float g, const float h, const float i);

	/**
	*	@brief Checks if both floating values are equal or similar with a maximum difference of epsilon.
	*/
	bool equal(const float f1, const float f2);

	/**
	*	@return 1 or -1 depending on the value sign.
	*/
	template<typename T>
	int sign(const T value);
};

inline float BasicOperations::determinant2x2(const float a, const float b, const float c, const float d)
{
	return (a * d - b * c);
}

inline float BasicOperations::determinant3x3(const float a, const float b, const float c, const float d, const float e, const float f, const float g, const float h, const float i)
{
	return (a * e * i + d * h * c + b * f * g - c * e * g - a * f * h - d * b * i);
}

inline bool BasicOperations::equal(const float f1, const float f2)
{
	return std::abs(f1 - f2) < glm::epsilon<float>();
}

template<typename T>
int BasicOperations::sign(const T value)
{
	return (T(0) < value) - (value < T(0));
}