#version 450

// ********** PARAMETERS & VARIABLES ***********

layout(location = 0) in vec3 vPosition;

// Matrices
uniform mat4 mModelViewProj;

void main()
{
	gl_Position = mModelViewProj * vec4(vPosition, 1.0f);
}