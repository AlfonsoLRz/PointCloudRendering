#version 450

#extension GL_ARB_compute_variable_group_size: enable

layout (local_size_variable) in;

layout(std430, binding = 0) buffer IndexBuffer { uint indexPoint[]; };

uniform uint arraySize;

void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= arraySize) return;

	indexPoint[index] = index;
}