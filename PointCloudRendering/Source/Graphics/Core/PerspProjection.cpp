#include "stdafx.h"
#include "PerspProjection.h"
#include "Graphics/Core/Camera.h"

glm::mat4 PerspProjection::calculateProjectionMatrix(Camera* camera)
{
	return glm::perspective(camera->getFovY(), camera->getAspect(), camera->getZNear(), camera->getZFar());
}

void PerspProjection::zoom(Camera* camera, const float speed)
{
	float newAngle = camera->getFovY() - speed;

	if (newAngle < glm::pi<float>() && newAngle > 0.0f)
	{
		camera->setFovY(newAngle);
	}
}
