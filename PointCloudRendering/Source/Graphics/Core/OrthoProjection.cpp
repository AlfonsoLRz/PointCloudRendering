#include "stdafx.h"
#include "OrthoProjection.h"

#include "Graphics/Core/Camera.h"

glm::mat4 OrthoProjection::calculateProjectionMatrix(Camera* camera)
{
	glm::vec2 bottomLeftCorner = camera->getBottomLeftCorner();
	
	return glm::ortho(bottomLeftCorner.x, -bottomLeftCorner.x, bottomLeftCorner.y, -bottomLeftCorner.y, camera->getZNear(), camera->getZFar());
}

void OrthoProjection::zoom(Camera* camera, const float speed)
{
	const float units			= -speed;
	const float raspect			= camera->getAspect();
	glm::vec2 bottomLeftCorner	= camera->getBottomLeftCorner();

	if (bottomLeftCorner.x - units >= 0 || bottomLeftCorner.y - units >= 0)
	{
		return;
	}

	bottomLeftCorner -= vec2(units * raspect, units);
	camera->setBottomLeftCorner(bottomLeftCorner);
}
