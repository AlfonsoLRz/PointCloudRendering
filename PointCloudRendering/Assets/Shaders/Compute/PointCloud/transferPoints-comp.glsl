#version 450

#extension GL_ARB_compute_variable_group_size: enable

layout (local_size_variable) in;

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout(std430, binding = 0) buffer InputBuffer	{ PointModel	inputPoint[]; };
layout(std430, binding = 1) buffer OutputBuffer { PointModel	outcomePoint[]; };
layout(std430, binding = 2) buffer IndexBuffer	{ uint			indexPoint[]; };

uniform uint arraySize;

void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= arraySize) return;

	outcomePoint[index] = inputPoint[indexPoint[index]];
}