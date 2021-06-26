#version 450

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

#define ROTATION_WEIGHT 1.0f / 40.0f

layout (std430, binding = 0) buffer PositionBuffer	{ vec4 posData[]; };
layout (std430, binding = 1) buffer ScaleBuffer		{ vec4 scaleData[]; };
layout (std430, binding = 2) buffer RotationBuffer	{ vec4 rotData[]; };
layout (std430, binding = 3) buffer RandomPosBuffer { vec4 randomPos[]; };
layout (std430, binding = 4) buffer CountTreeBuffer { uint instantiatedTrees; };

#define PI 3.1415926535f

uniform uint numTrees;
uniform uint offset;

// Textures
uniform sampler2D vegetationMap;
uniform sampler2D heightMap;

// Parameters to compute position
uniform float width, depth;
uniform mat4 modelMatrix;
uniform float terrainDisp;

// Scale
uniform float minScale, maxScale;

// Instancing
uniform float instanceBoundary;
uniform uint numInstantiatedTrees;

// Random number
float rand(vec2 co) {
	return fract(sin(dot(co, vec2(12.9898f, 78.233f))) * 43758.5453f);
}

void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= numTrees)
	{
		return;
	}

	vec2 randomPos = randomPos[index + offset].xy;
	float prob = texture(vegetationMap, randomPos).r;

	if (prob >= instanceBoundary)
	{
		uint treeID = atomicAdd(instantiatedTrees, 1);

		vec4 origPos = vec4(randomPos.x * width - width / 2.0f, .0f, (1.0f - randomPos.y) * depth - depth / 2.0f, 1.0f);
		origPos = modelMatrix * origPos;
		origPos = origPos + vec4(.0, 1.0f, .0f, .0f) * texture(heightMap, randomPos).r * terrainDisp;

		float randScale = rand(randomPos);
		randScale = randScale * (maxScale - minScale) + minScale;

		vec3 randRot = vec3(rand(randomPos.xy), rand(origPos.yx), rand(origPos.xy));

		posData[treeID] = vec4(origPos.xyz, 1.0f);
		scaleData[treeID] = vec4(randScale, randScale, randScale, 1.0f);
		rotData[treeID] = vec4((randRot.x * PI) * ROTATION_WEIGHT, randRot.y * 2.0f * PI, (randRot.z * PI) * ROTATION_WEIGHT, 1.0f);
	}
}