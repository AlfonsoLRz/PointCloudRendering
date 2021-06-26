#include "stdafx.h"
#include "AmbientLight.h"

void AmbientLight::applyLight(Light* light, RenderingShader* shader, const mat4& viewMatrix, const GLuint shad1, const GLuint shad2)
{
	shader->setSubroutineUniform(shad1, "lightUniform", "ambientLight");
	shader->setSubroutineUniform(shad2, "lightUniform", "ambientLight");

	shader->setUniform("Ia", light->getIa());
}

void AmbientLight::applyLight4ColouredPoints(Light* light, RenderingShader* shader, const mat4& viewMatrix)
{
	shader->setSubroutineUniform(GL_VERTEX_SHADER, "lightUniform", "ambientLight");

	shader->setUniform("Ia", light->getIa());
}
