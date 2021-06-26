#version 450

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

layout(std430, binding = 0) buffer OneBuffer { uint pBits[]; };
layout(std430, binding = 1) buffer ZeroBuffer { uint nBits[]; };
layout (std430, binding = 2) buffer InputBufferB { uint inIndices[]; };
layout (std430, binding = 3) buffer OutputBuffer { uint outIndices[]; };


uniform uint arraySize;


void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= arraySize)
	{
		return;
	}

	uint position;
	if (pBits[index] == 0)
	{
		position = nBits[index];
	}
	else
	{
		position = index - nBits[index] + nBits[arraySize - 1] + (pBits[arraySize - 1] ^ 1);			// nBits tells us the number of 1 before an index. If last position of nBits is 1 we need also to sum 1
	}

	outIndices[position] = inIndices[index];
}