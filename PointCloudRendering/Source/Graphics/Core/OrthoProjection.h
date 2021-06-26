#pragma once

#include "Graphics/Core/CameraProjection.h"

/**
*	@file OrthoProjection.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/10/2019
*/

/**
*	@brief Defines orthographic projection operations.
*/
class OrthoProjection: public CameraProjection
{
public:
	/**
	*	@brief Calculates the projection matrix following the orthographic principles.
	*	@param camera Camera with the parameters needed to calculate the projection matrix.
	*	@return Projection matrix.
	*/
	virtual glm::mat4 calculateProjectionMatrix(Camera* camera);

	/**
	*	@brief Getting closer / further to focus point.
	*	@param camera Camera with the parameters needed to calculate the projection matrix.
	*	@param speed Determines the amount of units to zoom the camera.
	*/
	virtual void zoom(Camera* camera, const float speed);
};

