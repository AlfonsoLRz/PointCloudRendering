#version 450

layout (location = 0) in vec4 vPosition;

uniform mat4		mModel;
uniform mat4		mModelViewProj;

// Textures
uniform sampler2D	heightMap;

// Parameters to compute position
uniform float		width, depth;
uniform float		terrainDisp;

out vec2 textCoord;


void main() {
	vec4 origPos = vec4(vPosition.x * width - width / 2.0f, .0f, (1.0f - vPosition.z) * depth - depth / 2.0f, 1.0f);
	origPos = origPos + vec4(.0, 1.01f, .0f, .0f) * texture(heightMap, vPosition.xz).r * terrainDisp;

	textCoord = vPosition.xz;
	gl_Position = mModelViewProj * origPos;
}