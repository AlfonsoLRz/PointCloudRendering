#include "stdafx.h"
#include "Camera.h"

#include "Geometry/3D/Vector3.h"
#include "Graphics/Core/OrthoProjection.h"
#include "Graphics/Core/PerspProjection.h"

// [ Static parameters initialization]

std::vector<std::shared_ptr<CameraProjection>> Camera::CAMERA_PROJECTION { std::shared_ptr<CameraProjection>(new PerspProjection()), 
																		   std::shared_ptr<CameraProjection>(new OrthoProjection()) };

const vec3 Camera::EYE					= vec3(0.0f, 3.0f, 10.0f);
const vec3 Camera::LOOK_AT				= vec3(0.0f, 3.0f, 0.0f);
const vec3 Camera::UP					= vec3(0.0f, 1.0f, 0.0f);

const float Camera::ZNEAR				= 0.1f;
const float Camera::ZFAR				= 200.0f;

const float Camera::FOV_X				= 80.0f * glm::pi<float>() / 180.0f;

const vec2 Camera::CANONICAL_VOL_CORNER = vec2(-5.0f, -5.0f);


/// [Public methods]

Camera::Camera(const uint16_t width, const uint16_t height): _backupCamera(nullptr)
{
	_eye	= EYE;
	_lookAt = LOOK_AT;
	_up		= UP;

	_zNear	= ZNEAR;
	_zFar	= ZFAR;

	_width	= width;
	_height = height;
	_aspect = this->computeAspect();
	_bottomLeftCorner = vec2(CANONICAL_VOL_CORNER.x * _aspect, CANONICAL_VOL_CORNER.y);

	_fovX	= FOV_X;
	_fovY	= this->computeFovY();

	_cameraType = PERSPECTIVE_PROJ;

	this->computeAxes(_n, _u, _v);
	_viewMatrix = this->computeViewMatrix();
	_projectionMatrix = CAMERA_PROJECTION[_cameraType]->calculateProjectionMatrix(this);
	_viewProjectionMatrix = _projectionMatrix * _viewMatrix;

	this->saveCamera();
}

Camera::Camera(const Camera& camera) : _backupCamera(nullptr)
{
	this->copyCameraAttributes(&camera);
}

Camera::~Camera()
{
	delete _backupCamera;
}

Camera& Camera::operator=(const Camera& camera)
{
	if (this != &camera)
	{
		this->copyCameraAttributes(&camera);
	}

	return *this;
}

void Camera::reset()
{
	this->copyCameraAttributes(_backupCamera);
}

void Camera::saveCamera()
{
	delete _backupCamera;

	_backupCamera = new Camera(*this);
}

void Camera::setBottomLeftCorner(const vec2& bottomLeft)
{
	_bottomLeftCorner = bottomLeft;

	this->computeProjectionMatrices();
}

void Camera::setCameraType(const CameraTypes cameraType)
{
	_cameraType = cameraType;

	this->computeViewMatrices();
	this->computeProjectionMatrices();
}

void Camera::setFovX(const float fovX)
{
	_fovX = fovX;
	_fovY = this->computeFovY();

	this->computeProjectionMatrices();
}

void Camera::setFovY(const float fovY)
{
	_fovY = fovY;

	this->computeProjectionMatrices();
}

void Camera::setLookAt(const vec3& position)
{
	_lookAt = position;

	this->computeAxes(_n, _u, _v);
	this->computeViewMatrices();
}

void Camera::setPosition(const vec3& position)
{
	_eye = position;

	this->computeAxes(_n, _u, _v);
	this->computeViewMatrices();
}

void Camera::setRaspect(const uint16_t width, const uint16_t height)
{
	_width	= width;
	_height = height;
	_aspect = this->computeAspect();
	_bottomLeftCorner = vec2(_bottomLeftCorner.y * _aspect, _bottomLeftCorner.y);

	this->computeProjectionMatrices();
}

void Camera::setUp(const vec3& up)
{
	_up = up;

	this->computeViewMatrices();
}

void Camera::setZFar(const float zfar)
{
	_zFar = zfar;

	this->computeProjectionMatrices();
}

void Camera::setZNear(const float znear)
{
	_zNear = znear;

	this->computeProjectionMatrices();
}

// [Movements] 

void Camera::boom(float speed)
{
	const glm::mat4 translationMatrix = glm::translate(mat4(1.0f), _v * speed);			// Translation in y axis

	_eye	= vec3(translationMatrix * vec4(_eye, 1.0f));
	_lookAt = vec3(translationMatrix * vec4(_lookAt, 1.0f));

	this->computeViewMatrices();
}

void Camera::crane(float speed)
{
	boom(-speed);					// Implemented as another method to take advantage of nomenclature
}	

void Camera::dolly(float speed)
{
	const mat4 translationMatrix = glm::translate(mat4(1.0f), -_n * speed);			// Translation in z axis
	_eye	= vec3(translationMatrix * vec4(_eye, 1.0f));
	_lookAt = vec3(translationMatrix * vec4(_lookAt, 1.0f));

	this->computeViewMatrices();
}

void Camera::orbitXZ(float speed)
{
	const mat4 rotationMatrix = glm::rotate(mat4(1.0f), speed, _u);				// We will pass over the scene, x or z axis could be used
	
	/**
	- Just in case we want to avoid this orbit movement to get over the y axis. At this moment it's implemented as a fully free movement
	float alpha = glm::acos(glm::dot(_n, vec3(0.0f, 1.0f, 0.0f)));
	if (-speed > alpha || (glm::pi<float>() - alpha) < speed)
	{
		return;
	}
	*/

	_eye	= vec3(rotationMatrix * vec4(_eye - _lookAt, 1.0f)) + _lookAt;
	_u		= vec3(rotationMatrix * vec4(_u, 0.0f));		
	_v		= vec3(rotationMatrix * vec4(_v, 0.0f));
	_n		= vec3(rotationMatrix * vec4(_n, 0.0f));
	_up		= glm::normalize(glm::cross(_n, _u));						// Free rotation => we can look down or up

	this->computeViewMatrices();
}

void Camera::orbitY(float speed)
{
	const mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), speed, glm::vec3(0.0, 1.0f, 0.0f));

	_u		= vec3(rotationMatrix * vec4(_u, 0.0f));
	_v		= vec3(rotationMatrix * vec4(_v, 0.0f));
	_n		= vec3(rotationMatrix * vec4(_n, 0.0f));
	_up		= glm::normalize(glm::cross(_n, _u));								// This movement doesn't change UP, but it could occur as a result of previous operations
	_eye	= vec3(rotationMatrix * vec4(_eye - _lookAt, 1.0f)) + _lookAt;

	this->computeViewMatrices();
}

void Camera::pan(float speed)
{
	const mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), speed, vec3(0.0f, 1.0f, 0.0f));

	/**
	*	Up vector can change, not in the original position tho. Example: orbit XZ (rotated camera) + pan
	*/
	_u		= vec3(rotationMatrix * vec4(_u, 0.0f));
	_v		= vec3(rotationMatrix * vec4(_v, 0.0f));
	_n		= vec3(rotationMatrix * vec4(_n, 0.0f));
	_up		= glm::normalize(glm::cross(_n, _u));
	_lookAt = vec3(rotationMatrix * vec4(_lookAt - _eye, 1.0f)) + _eye;	

	this->computeViewMatrices();
}

void Camera::tilt(float speed)
{
	const mat4 rotationMatrix = glm::rotate(mat4(1.0f), speed, _u);

	const vec3 n = glm::vec3(rotationMatrix * glm::vec4(_n, 0.0f));							
	float alpha = glm::acos(glm::dot(n, glm::vec3(0.0f, 1.0f, 0.0f)));

	if (alpha < speed || alpha > (glm::pi<float>() - speed))
	{
		return;
	}

	_v		= glm::vec3(rotationMatrix * glm::vec4(_v, 0.0f));		
	_n		= n;														
	_up		= glm::normalize(glm::cross(_n, _u));											// It could change because of the rotation
	_lookAt = glm::vec3(rotationMatrix * glm::vec4(_lookAt - _eye, 1.0f)) + _eye;

	this->computeViewMatrices();
}

void Camera::truck(float speed)
{
	const mat4 translationMatrix = glm::translate(mat4(1.0f), _u * speed);				// Translation in x axis

	_eye	= vec3(translationMatrix * vec4(_eye, 1.0f));
	_lookAt = vec3(translationMatrix * vec4(_lookAt, 1.0f));

	this->computeViewMatrices();
}

void Camera::zoom(float speed)
{
	CAMERA_PROJECTION[_cameraType]->zoom(this, speed);
}

/// [Private methods]

float Camera::computeAspect()
{
	return (float)_width / _height;
}

void Camera::computeAxes(vec3& n, vec3& u, vec3& v)
{
	n = glm::normalize(_eye - _lookAt);							// z axis
	
	if (Vector3::equal(n,-_up))		// x axis: UP x n is 0 as both vectors are parallel. Since up and n are normalized we can check if they are equal (with epsilon checkup)
	{
		u = glm::normalize(glm::cross(vec3(0.0f, 0.0f, -1.0f), n));
	}
	else if (Vector3::equal(n, _up))
	{
		u = glm::normalize(glm::cross(vec3(0.0f, 0.0f, 1.0f), n));
	}
	else
	{
		u = glm::normalize(glm::cross(_up, n));
	}
	v = glm::normalize(glm::cross(n, u));					// y axis
}

vec2 Camera::computeBottomLeftCorner()
{
	const float halfWidth	= _width / 2.0f;
	const float halfHeight	= _height / 2.0f;

	return vec2(-halfWidth, -halfHeight);
}

float Camera::computeFovY()
{
	return 2.0f * glm::atan(glm::tan(_fovX / 2.0f) / _aspect);
}

void Camera::computeProjectionMatrices()
{
	_projectionMatrix = CAMERA_PROJECTION[_cameraType]->calculateProjectionMatrix(this);
	_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
}

void Camera::computeViewMatrices()
{
	_viewMatrix = computeViewMatrix();
	_viewProjectionMatrix = _projectionMatrix * _viewMatrix;
}

glm::mat4 Camera::computeViewMatrix()
{
	//std::cout << "Position: " << _eye.x << ", " << _eye.y << ", " << _eye.z << "; look at: " << _lookAt.x << ", " << _lookAt.y << ", " << _lookAt.z << std::endl;

	return glm::lookAt(_eye, _lookAt, _up);
}

void Camera::copyCameraAttributes(const Camera* camera)
{
	this->_eye		= camera->_eye;
	this->_lookAt	= camera->_lookAt;
	this->_up		= camera->_up;

	this->_zNear	= camera->_zNear;
	this->_zFar		= camera->_zFar;

	this->_aspect	= camera->_aspect;
	this->_width	= camera->_width;
	this->_height	= camera->_height;

	this->_n		= camera->_n;
	this->_u		= camera->_u;
	this->_v		= camera->_v;

	this->_viewMatrix			= camera->_viewMatrix;
	this->_projectionMatrix		= camera->_projectionMatrix;
	this->_viewProjectionMatrix = camera->_viewProjectionMatrix;

	this->_fovX = camera->_fovX;
	this->_fovY = camera->_fovY;

	this->_bottomLeftCorner = camera->_bottomLeftCorner;

	if (camera->_backupCamera)
	{
		delete this->_backupCamera;
		this->_backupCamera = new Camera(*camera->_backupCamera);
	}
}