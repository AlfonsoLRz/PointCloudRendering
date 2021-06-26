#pragma once

/**
*	@file Vector3.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 10/07/2019
*/

/**
*	@brief Wrapper for operations with vector of 3 coordinates.
*/
class Vector3
{
public:
	/**
	*	@brief Checks if both vectors are the same with epsilon comparison.
	*/
	static bool equal(const vec3& v1, const vec3& v2);
};

