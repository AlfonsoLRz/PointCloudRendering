#pragma once

#include "stdafx.h"
#include "Graphics/Core/Light.h"
#include "Graphics/Core/ShaderProgram.h"

/**
*	@file Light.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/14/2019
*/

/**
*	@brief Specifies the uniform variables for an implementation of light attenuation type.
*/
class LightAttenuation
{
public:
	/**
	*	@brief Applies the uniform variables associated to an specific attenuation formula. To be defined.
	*	@param light Light from where we get the parameters.
	*	@param shader Shader where we specity the uniform variables.
	*/
	virtual void applyAttenuation(Light* light, RenderingShader* shader) = 0;

	/**
	*	@brief Applies the uniform variables associated to an specific attenuation formula.
	*	@param light Light from where we get the parameters.
	*	@param shader Shader where we specity the uniform variables.
	*/
	virtual void applyAttenuation4ColouredPoints(Light* light, RenderingShader* shader) = 0;
};

