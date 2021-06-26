#version 450

in vec3 normal;

layout (location = 0) out vec4 fNormal;


void main()
{
	fNormal = vec4(normalize(normal), 1.0f);
}
