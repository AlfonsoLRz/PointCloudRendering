#include "stdafx.h"
#include "RimLight.h"

void RimLight::applyLight(Light* light, RenderingShader* shader, const mat4& viewMatrix, const GLuint shad1, const GLuint shad2)
{
	shader->setSubroutineUniform(shad1, "lightUniform", "rimLight");
	shader->setSubroutineUniform(shad2, "lightUniform", "rimLight");

	shader->setUniform("Ia", light->getIa());
}

void RimLight::applyLight4ColouredPoints(Light* light, RenderingShader* shader, const mat4& viewMatrix)
{
	shader->setSubroutineUniform(GL_VERTEX_SHADER, "lightUniform", "rimLight");

	shader->setUniform("Ia", light->getIa());
}
