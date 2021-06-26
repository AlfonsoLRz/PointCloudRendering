#version 450

in vec2 textCoord;

// Textures
uniform sampler2D	densityMap;
uniform sampler2D	colorMap;

layout (location = 0) out vec4 fColor;

void main() {
	fColor = vec4(texture(colorMap, vec2(.5f, texture(densityMap, textCoord.yx).r)).rgb, 1.0f);
}