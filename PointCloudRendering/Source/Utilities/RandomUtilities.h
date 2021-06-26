#pragma once

#include "stdafx.h"

/**
*	@file RandomUtilities.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 01/02/2021
*/

typedef std::mt19937_64							RandomNumberGenerator;
typedef std::uniform_real_distribution<double>	DoubleUniformDistribution;

/**
*	@brief Utilities which help us to retrieve random values.
*	@author Alfonso López Ruiz.
*/
namespace RandomUtilities
{
	//!< Private members
	namespace
	{
		RandomNumberGenerator		_randomNumberGenerator;
		DoubleUniformDistribution	_uniformDistribution;
	}

	/**
	*	@return New random value retrieved from a random uniform distribution.
	*/
	double getUniformRandomValue();

	/**
	*	@brief Initializes the uniform distribution so that it gets a new seed and range.
	*/
	void initializeUniformDistribution(const float min, const float max);
}

inline double RandomUtilities::getUniformRandomValue()
{
	return _uniformDistribution(_randomNumberGenerator);
}

inline void RandomUtilities::initializeUniformDistribution(const float min, const float max)
{
	uint64_t			timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::seed_seq		seedSeq{ uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32) };
	
	_uniformDistribution = std::uniform_real_distribution<double>(min, max);
	_randomNumberGenerator.seed(seedSeq);
}
