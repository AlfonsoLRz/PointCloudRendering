#pragma once

#include "Graphics/Core/ShaderProgram.h"

/**
*	@file RenderingShader.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/13/2019
*/

/**
*	@brief Shader to render an scene, and so can only integrates the three types of shaders related to that action.
*/
class RenderingShader: public ShaderProgram
{
public:			
	/**
	*	@brief Default constructor.
	*/
	RenderingShader();

	/**
	*	@brief Destructor.
	*/
	virtual ~RenderingShader();

	/**
	*	@brief Applies all the active subroutines.
	*/
	virtual void applyActiveSubroutines();

	/**
	*	@brief Reads and compiles all the shaders which contain the name specified as an argument.
	*	@filename filename Shader program name. Example: pointCloud => pointCloud-vert.glsl, pointCloud-frag.glsl...
	*/
	virtual GLuint createShaderProgram(const char* filename);
};

