#pragma once

/**
*	@file PointCloudParameters.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 30/06/2021
*/

/**
*	@brief 
*/
struct PointCloudParameters
{
public:
	inline static float		_distanceThreshold = 1.01f;			//!<
	inline static bool		_enableHQR = true;					//!<
	inline static bool		_sortPointCloud = true;				//!<
	inline static bool		_reducePointCloud = false;			//!<
	inline static GLuint	_reduceIterations = 1;				//!<
};