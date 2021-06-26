#version 450

in vec3 position;

layout (location = 0) out vec4 fPosition;


void main()
{
	fPosition = vec4(position, 1.0f);
}
