#version 450

layout(location = 0) in vec3 vPosition;

// Matrices
uniform mat4 mModelView;
uniform mat4 mModelViewProj;

out vec3 position;


void main()
{
	position = vec3(mModelView *  vec4(vPosition, 1.0f));
	gl_Position = mModelViewProj * vec4(vPosition, 1.0f);
}
