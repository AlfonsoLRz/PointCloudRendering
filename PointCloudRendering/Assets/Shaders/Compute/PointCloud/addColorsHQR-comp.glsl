#version 450

#extension GL_ARB_compute_variable_group_size : enable
#extension GL_ARB_gpu_shader_int64 : require
#extension GL_NV_shader_atomic_int64 : require
#extension GL_NV_gpu_shader5 : enable

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout (local_size_variable) in;

layout (std430, binding = 0) buffer DepthBuffer		{ uint			depthBuffer[]; };
layout (std430, binding = 1) buffer Color01Buffer	{ uint64_t		colorBuffer01[]; };
layout (std430, binding = 2) buffer Color02Buffer	{ uint64_t		colorBuffer02[]; };
layout (std430, binding = 3) buffer PointBuffer		{ PointModel	points[]; };

uniform float	distanceThreshold;
uniform float	normalThreshold;
uniform uint	numPoints;
uniform mat4	projectionMatrix;
uniform mat4	viewMatrix;
uniform uvec2	windowSize;

void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= numPoints) return;

	vec4 projectedPoint = projectionMatrix * vec4(points[index].point, 1.0f);
	projectedPoint.xyz /= projectedPoint.w;

	if (projectedPoint.w <= 0.0 || projectedPoint.x < -1.0 || projectedPoint.x > 1.0 || projectedPoint.y < -1.0 || projectedPoint.y > 1.0)
	{
		return;
	}

	vec3 n					= normalize(points[index].normal.xyz);
	const vec3 v			= normalize(-(viewMatrix * vec4(points[index].point, 1.0f)).xyz);
	const float vdn			= 1.0f - step(normalThreshold, abs(dot(v, n)));

	ivec2 windowPosition	= ivec2((projectedPoint.xy * 0.5f + 0.5f) * windowSize);
	int pointIndex			= int(windowPosition.y * windowSize.x + windowPosition.x);
	float depth				= projectedPoint.w;
	float depthInBuffer		= uintBitsToFloat(depthBuffer[pointIndex]);
	uvec3 rgbColor			= uvec3(unpackUnorm4x8(points[index].rgb).rgb * vdn * 255.0f);

	if (depth < depthInBuffer * distanceThreshold)			// Same surface
	{
		uint64_t rg = (uint64_t(rgbColor.r) << 32) | rgbColor.g;
		uint64_t ba = (uint64_t(rgbColor.b) << 32) | 1;

		atomicAdd(colorBuffer01[pointIndex], rg);
		atomicAdd(colorBuffer02[pointIndex], ba);
	}
}