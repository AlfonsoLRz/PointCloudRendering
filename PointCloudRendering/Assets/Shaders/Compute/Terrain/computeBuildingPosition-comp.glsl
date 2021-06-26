#version 450

#extension GL_ARB_compute_variable_group_size : enable
layout(local_size_variable) in;

layout(std430, binding = 0) buffer RandomPosBuffer	{ vec4 randomPosData[]; };
layout(std430, binding = 1) buffer PositionBuffer	{ vec4 positionData[]; };
layout(std430, binding = 2) buffer GoodnessBuffer	{ vec4 goodnessData[]; };

#define PI 3.1415926535f

uniform uint numPositions;

// Textures
uniform sampler2D vegetationMap;
uniform sampler2D heightMap;
uniform sampler2D normalMap;

// Parameters to compute position
uniform float	width, depth;
uniform mat4	modelMatrix;
uniform float	terrainDisp;

// Instancing
uniform float	instanceBoundary;

// Radius checking
uniform float	radius;

void computePosition(const uint index, vec2 randomPos)
{
	vec4 origPos = vec4(randomPos.x * width - width / 2.0f, .0f, (1.0f - randomPos.y) * depth - depth / 2.0f, 1.0f);
	origPos = modelMatrix * origPos;
	origPos = origPos + vec4(.0, 1.0f, .0f, .0f) * texture(heightMap, randomPos).r * terrainDisp;

	positionData[index] = vec4(origPos.xyz, 1.0f);
}

// Random number
float rand(vec2 co) {
	return fract(sin(dot(co, vec2(12.9898f, 78.233f))) * 43758.5453f);
}

void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= numPositions)
	{
		return;
	}

	vec2 randomPos = randomPosData[index].xy;
	float prob = texture(vegetationMap, randomPos).r;
	float goodness = .0f;

	vec2 mask[9];
	mask[0] = vec2(-1, 1) * radius;
	mask[1] = vec2(0, 1) * radius;
	mask[2] = vec2(1, 1) * radius;
	mask[3] = vec2(-1, 0) * radius;
	mask[4] = vec2(0, 0) * radius;
	mask[5] = vec2(1, 0) * radius;
	mask[6] = vec2(-1, -1) * radius;
	mask[7] = vec2(0, -1) * radius;
	mask[8] = vec2(1, -1) * radius;

	for (int i = 0; i < 9; ++i)
	{
		vec2 newPos = randomPos + mask[i];
		vec3 normal = vec3((2.0f * texture(normalMap, newPos)) - 1.0f);
		normal = vec3(normal.x, normal.z, normal.y);

		goodness += step(instanceBoundary, texture(vegetationMap, newPos).r);
		goodness += dot(normal, vec3(.0f, 1.0f, .0f));
	}

	goodnessData[index].x = goodness;

	computePosition(index, randomPos);
}