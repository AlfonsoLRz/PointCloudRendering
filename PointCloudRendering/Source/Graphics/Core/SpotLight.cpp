#include "stdafx.h"
#include "SpotLight.h"

void SpotLight::applyLight(Light* light, RenderingShader* shader, const mat4& viewMatrix, const GLuint shad1, const GLuint shad2)
{
	shader->setSubroutineUniform(shad1, "lightUniform", "spotLight");
	shader->setSubroutineUniform(shad2, "lightUniform", "spotLight");

	const float cosUmbraRad		= std::cos(light->getUmbraDegrees() * glm::pi<float>() / 180.0f);											// Rad translation
	const float cosPenumbraRad	= std::cos(light->getPenumbraDegrees() * glm::pi<float>() / 180.0f);

	shader->setUniform("lightPosition", vec3(viewMatrix * vec4(light->getPosition(), 1.0f)));					// Already transformed for every fragment
	shader->setUniform("lightDirection", vec3(viewMatrix * vec4(light->getDirection(), 0.0f)));
	shader->setUniform("cosUmbra", cosUmbraRad);
	shader->setUniform("cosPenumbra", cosPenumbraRad);
	shader->setUniform("exponentS", light->getExponentS());
	shader->setUniform("Id", light->getId());
	shader->setUniform("Is", light->getIs());
}

void SpotLight::applyLight4ColouredPoints(Light* light, RenderingShader* shader, const mat4& viewMatrix)
{
	shader->setSubroutineUniform(GL_VERTEX_SHADER, "lightUniform", "spotLight");

	const float cosUmbraRad		= std::cos(light->getUmbraDegrees() * glm::pi<float>() / 180.0f);											// Rad translation
	const float cosPenumbraRad	= std::cos(light->getPenumbraDegrees() * glm::pi<float>() / 180.0f);

	shader->setUniform("lightPosition", vec3(viewMatrix * vec4(light->getPosition(), 1.0f)));					// Already transformed for every fragment
	shader->setUniform("lightDirection", vec3(viewMatrix * vec4(light->getDirection(), 0.0f)));
	shader->setUniform("cosUmbra", cosUmbraRad);
	shader->setUniform("cosPenumbra", cosPenumbraRad);
	shader->setUniform("exponentS", light->getExponentS());
	shader->setUniform("Id", light->getId());
	shader->setUniform("Is", light->getIs());
}
