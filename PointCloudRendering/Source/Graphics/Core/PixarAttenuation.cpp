#include "stdafx.h"
#include "PixarAttenuation.h"

void PixarAttenuation::applyAttenuation(Light* light, RenderingShader* shader)
{
	auto coefficients = light->getKCoefficients();

	shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "attenuationUniform", "pixarAttenuation");

	shader->setUniform("fMax", light->getMaxAttFactor());
	shader->setUniform("fC", light->getFactorC());
	shader->setUniform("distC", light->getDistanceC());
	shader->setUniform("exponentSE", light->getExponentSE());
	shader->setUniform("k0", coefficients[0]);
	shader->setUniform("k1", coefficients[1]);
}

void PixarAttenuation::applyAttenuation4ColouredPoints(Light* light, RenderingShader* shader)
{
	auto coefficients = light->getKCoefficients();

	shader->setSubroutineUniform(GL_VERTEX_SHADER, "attenuationUniform", "pixarAttenuation");

	shader->setUniform("fMax", light->getMaxAttFactor());
	shader->setUniform("fC", light->getFactorC());
	shader->setUniform("distC", light->getDistanceC());
	shader->setUniform("exponentSE", light->getExponentSE());
	shader->setUniform("k0", coefficients[0]);
	shader->setUniform("k1", coefficients[1]);
}
