#pragma once

#include "stdafx.h"

class Camera;

/**
*	@file CameraProjection.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/10/2019
*/

/**
*	@brief Abstract class for any camera projection.
*/
class CameraProjection
{
public:
	/**
	*	@brief To be defined function: calculates the projection matrix by following the perspective or orthographic principles.
	*	@param camera Camera with the parameters needed to calculate the projection matrix.
	*	@return Projection matrix.
	*/
	virtual mat4 calculateProjectionMatrix(Camera* camera) = 0;

	/**
	*	@brief Getting closer / further to focus point.
	*	@param camera Camera with the parameters needed to calculate the projection matrix.
	*	@param speed Determines the amount of units to zoom the camera.
	*/
	virtual void zoom(Camera* camera, const float speed) = 0;
};

