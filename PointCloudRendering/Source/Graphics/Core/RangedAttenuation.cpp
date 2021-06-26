#include "stdafx.h"
#include "RangedAttenuation.h"

void RangedAttenuation::applyAttenuation(Light* light, RenderingShader* shader)
{
	auto distances = light->getDistanceRange();

	shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "attenuationUniform", "rangedAttenuation");

	shader->setUniform("minDistance", distances[0]);
	shader->setUniform("maxDistance", distances[1]);
}

void RangedAttenuation::applyAttenuation4ColouredPoints(Light* light, RenderingShader* shader)
{
	auto distances = light->getDistanceRange();

	shader->setSubroutineUniform(GL_VERTEX_SHADER, "attenuationUniform", "rangedAttenuation");

	shader->setUniform("minDistance", distances[0]);
	shader->setUniform("maxDistance", distances[1]);
}
