#include "stdafx.h"
#include "PointLight.h"

void PointLight::applyLight(Light* light, RenderingShader* shader, const mat4& viewMatrix, const GLuint shad1, const GLuint shad2)
{
	shader->setSubroutineUniform(shad1, "lightUniform", "pointLight");
	shader->setSubroutineUniform(shad2, "lightUniform", "pointLight");

	shader->setUniform("lightPosition", vec3(viewMatrix * vec4(light->getPosition(), 1.0f)));					// Already transformed for every fragment
	shader->setUniform("Id", light->getId());
	shader->setUniform("Is", light->getIs());
}

void PointLight::applyLight4ColouredPoints(Light* light, RenderingShader* shader, const mat4& viewMatrix)
{
	shader->setSubroutineUniform(GL_VERTEX_SHADER, "lightUniform", "pointLight");

	shader->setUniform("lightPosition", vec3(viewMatrix * vec4(light->getPosition(), 1.0f)));					// Already transformed for every fragment
	shader->setUniform("Id", light->getId());
	shader->setUniform("Is", light->getIs());
}
