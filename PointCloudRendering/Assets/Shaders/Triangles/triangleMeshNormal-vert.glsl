#version 450

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;

// Matrices
uniform mat4 mModelView;
uniform mat4 mModelViewProj;

out vec3 normal;


void main()
{
	gl_Position = mModelViewProj * vec4(vPosition, 1.0f);
	normal = vec3(mModelView * vec4(vNormal, 0.0f));
}
