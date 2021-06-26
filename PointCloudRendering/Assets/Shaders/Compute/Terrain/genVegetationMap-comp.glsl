#version 450

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

layout (std430, binding = 0) buffer VegetationMapBuffer	{ vec4 vegMap[]; };

// Textures
uniform sampler2D baseNoiseMap;
uniform sampler2D heightMap;
uniform sampler2D normalMap;

// Parameters to modify base noise map
uniform uint  globalSize;
uniform float minHeight;
uniform float maxHeight;
uniform float heightWeight;
uniform float slopeWeight;

// Random number
float rand(const vec2 co)
{
	return (10.0f * 2.0f * fract(sin(dot(co, vec2(12.9898f, 78.233f))) * 43758.5453f) / 100.0f) - 0.1f;		// 10% weight
}

void main()
{
	const uvec2 pos = gl_GlobalInvocationID.xy;
	const uint index = pos.y * globalSize + pos.x;

	if (pos.x >= globalSize || pos.y >= globalSize)
	{
		return;
	}

	vec2 textCoord = vec2(pos) / float(globalSize);
	float noiseValue = texture(baseNoiseMap, textCoord).r;
	float height = texture(heightMap, textCoord).r;
	vec3 normal = texture(normalMap, textCoord).rgb;

	// Reduce probability with height
	float heightProb = (1.0f - smoothstep(minHeight, maxHeight, height)) * heightWeight;

	// Reduce probability with slope
	float dotNormalX = dot(normal, vec3(.0f, .0f, 1.0f));
	float slopeProb = (1.0f - dotNormalX) * slopeWeight;

	vegMap[index] = vec4(vec3(clamp(1.0f - heightProb - slopeProb, .0f, 1.0f)), 1.0f);
}