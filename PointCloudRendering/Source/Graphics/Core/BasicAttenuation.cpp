#include "stdafx.h"
#include "BasicAttenuation.h"

void BasicAttenuation::applyAttenuation(Light* light, RenderingShader* shader)
{
	auto coefficients = light->getAttCoefficients();

	shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "attenuationUniform", "basicAttenuation");

	shader->setUniform("c1", coefficients[0]);
	shader->setUniform("c2", coefficients[1]);
	shader->setUniform("c3", coefficients[2]);
}

void BasicAttenuation::applyAttenuation4ColouredPoints(Light* light, RenderingShader* shader)
{
	auto coefficients = light->getAttCoefficients();

	shader->setSubroutineUniform(GL_VERTEX_SHADER, "attenuationUniform", "basicAttenuation");

	shader->setUniform("c1", coefficients[0]);
	shader->setUniform("c2", coefficients[1]);
	shader->setUniform("c3", coefficients[2]);
}
