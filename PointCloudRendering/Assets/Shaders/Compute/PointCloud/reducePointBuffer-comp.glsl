#version 450

#extension GL_ARB_compute_variable_group_size: enable
#extension GL_NV_shader_thread_group : require
#extension GL_NV_shader_thread_shuffle : require
#extension GL_ARB_shader_group_vote : require
#extension GL_ARB_shader_ballot : require

layout (local_size_variable) in;

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout(std430, binding = 0) buffer InputBuffer		{ PointModel	point[]; };
layout(std430, binding = 1) buffer IndexBuffer		{ uint			indexPoint[]; };
layout(std430, binding = 2) buffer PointCounter		{ uint			numPoints; };
layout(std430, binding = 3) buffer PointAuxCounter  { uint			numPointsAux; };

uniform vec3	sceneMaxBoundary, sceneMinBoundary;			// Scene AABB to normalize positions

// Expands a 10-bit integer into 30 bits by inserting 2 zeros after each bit.
uint expandBits(in uint v)
{
	v = (v * 0x00010001u) & 0xFF0000FFu;
	v = (v * 0x00000101u) & 0x0F00F00Fu;
	v = (v * 0x00000011u) & 0xC30C30C3u;
	v = (v * 0x00000005u) & 0x49249249u;

	return v;
}

// We computes Morton codes for points located within the unit cube [0, 1].
vec3 normalizeScenePoint(const vec3 position)
{
	return (position - sceneMinBoundary) / (sceneMaxBoundary - sceneMinBoundary);
}

// Calculates a 30-bit Morton code for the given 3D point located within the unit cube [0,1]. 2^10 = 1024, 10 bits for each coordinate.
uint morton3D(const vec3 position)
{
	const vec3 normVertex = normalizeScenePoint(position);
	const float x = normVertex.x * 1024.0f;
	const float y = normVertex.y * 1024.0f;
	const float z = normVertex.z * 1024.0f;

	uint xx = expandBits(uint(x));
	uint yy = expandBits(uint(y));
	uint zz = expandBits(uint(z));

	return xx * 4 + yy * 2 + zz;
}


void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= numPoints) return;

	const uint cIndexPoint = indexPoint[index];
	const uint mortonCode = morton3D(point[cIndexPoint].point);

	uint headID = shuffleNV(mortonCode, 0, 32);
	uint activeThreads = ballotThreadNV(headID == mortonCode);
	uint firstThread = findLSB(activeThreads);

	if (firstThread == gl_ThreadInWarpNV)
	{
		const uint atomicIndex = atomicAdd(numPointsAux, 1);
		indexPoint[atomicIndex] = cIndexPoint;
	}
}