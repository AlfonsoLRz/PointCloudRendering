#include "stdafx.h"
#include "DirectionalLight.h"

void DirectionalLight::applyLight(Light* light, RenderingShader* shader, const mat4& viewMatrix, const GLuint shad1, const GLuint shad2)
{
	shader->setSubroutineUniform(shad1, "lightUniform", "directionalLight");
	shader->setSubroutineUniform(shad2, "lightUniform", "directionalLight");

	shader->setUniform("lightDirection", vec3(viewMatrix * vec4(light->getDirection(), 0.0f)));					// Already transformed for every fragment
	shader->setUniform("Id", light->getId());
	shader->setUniform("Is", light->getIs());
}

void DirectionalLight::applyLight4ColouredPoints(Light* light, RenderingShader* shader, const mat4& viewMatrix)
{
	shader->setSubroutineUniform(GL_VERTEX_SHADER, "lightUniform", "directionalLight");

	shader->setUniform("lightDirection", vec3(viewMatrix * vec4(light->getDirection(), 0.0f)));					// Already transformed for every fragment
	shader->setUniform("Id", light->getId());
	shader->setUniform("Is", light->getIs());
}
