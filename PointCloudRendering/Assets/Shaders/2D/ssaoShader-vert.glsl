#version 450

layout (location = 2) in vec2 vTextCoord;

out vec2 textCoord;

void main()
{
	textCoord = vTextCoord;

	const vec2 position = vTextCoord * 2.0f - 1.0f;	
    gl_Position = vec4(vec3(position, 0.0f), 1.0f);
}