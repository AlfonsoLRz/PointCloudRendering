#version 450

uniform vec3 vColor;

layout (location = 0) out vec4 fColor;
layout (location = 1) out vec4 brightColor;

void main() {
	fColor = vec4(vColor, 1.0f);
	brightColor = vec4(0.0f);
}