#pragma once

#include "Graphics/Core/Camera.h"

/**
*	@file CameraManager.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/11/2019
*/

/**
*	@brief Manager for all the cameras which are available in this application.
*/
class CameraManager
{
protected:
	uint8_t									_activeCamera;					//!< Active camera which captures the scene
	std::vector<std::unique_ptr<Camera>>	_camera;						//!< Currently available cameras, where only of them is active

public:
	/**
	*	@brief Default constructor. Starts with no cameras.
	*/
	CameraManager();

	/**
	*	@brief Destructor. Frees camera resources.
	*/
	virtual ~CameraManager();

	/**
	*	@brief Deletes a camera from the array.
	*	@param cameraIndex Index of the camera to be deleted.
	*/
	void deleteCamera(uint8_t cameraIndex);

	/**
	*	@return Active camera on the scene.
	*/
	Camera* getActiveCamera() const;

	/**
	*	@return Camera saved at the specified index. This is the method to modify the parameters of any camera, active or not.
	*	@param cameraIndex Index of the camera to be returned.
	*/
	Camera* getCamera(uint8_t cameraIndex) const;

	/**
	*	@brief Inserts a new camera in the array.
	*	@param camera Already created camera to be included.
	*/
	void insertCamera(Camera* camera);

	/**
	*	@brief Modifies the currently active camera.
	*	@param cameraIndex Index where the new active camera is placed at.
	*/
	void setActiveCamera(uint8_t cameraIndex);
};

