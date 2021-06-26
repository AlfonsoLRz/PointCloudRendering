#include "stdafx.h"
#include "CameraManager.h"

/// [Public methods]

CameraManager::CameraManager(): _activeCamera(0)
{
}

CameraManager::~CameraManager()
{
}

void CameraManager::deleteCamera(uint8_t cameraIndex)
{
	if (cameraIndex >= _camera.size()) return;

	delete _camera[cameraIndex].get();
}

Camera* CameraManager::getActiveCamera() const
{
	if (_activeCamera >= _camera.size()) return nullptr;

	return _camera[_activeCamera].get();
}

Camera* CameraManager::getCamera(uint8_t cameraIndex) const
{
	if (cameraIndex >= _camera.size()) return nullptr;

	return _camera[cameraIndex].get();
}

void CameraManager::insertCamera(Camera* camera)
{
	camera->saveCamera();
	_camera.push_back(std::unique_ptr<Camera>(camera));
}

void CameraManager::setActiveCamera(uint8_t cameraIndex)
{
	if (cameraIndex >= _camera.size()) return;

	_activeCamera = cameraIndex;
}
