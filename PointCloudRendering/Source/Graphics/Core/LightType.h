#pragma once

#include "stdafx.h"
#include "Graphics/Core/Light.h"
#include "Graphics/Core/RenderingShader.h"

/**
*	@file LightType.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/14/2019
*/

/**
*	@brief Specifies the uniform variables for a light, taking into account which type of light is chosen.
*/
class LightType
{
public:
	/**
	*	@brief Applies the uniform variables related to an specific type of light. To be defined.
	*	@param light Light from where we get the parameters.
	*	@param shader Shader where we specity the uniform variables.
	*	@param viewMatrix View matrix of currently active camera.
	*/
	virtual void applyLight(Light* light, RenderingShader* shader, const mat4& viewMatrix, const GLuint shad1 = GL_VERTEX_SHADER, const GLuint shad2 = GL_FRAGMENT_SHADER) = 0;

	/**
	*	@brief Applies the uniform variables related to an specific type of light. To be defined.
	*	@param light Light from where we get the parameters.
	*	@param shader Shader where we specity the uniform variables.
	*	@param viewMatrix View matrix of currently active camera.
	*/
	virtual void applyLight4ColouredPoints(Light* light, RenderingShader* shader, const mat4& viewMatrix) = 0;
};

