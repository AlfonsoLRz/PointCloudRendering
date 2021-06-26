#version 450

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout (std430, binding = 0) buffer MortonCodesBuffer { uint mortonCodes[]; };
layout (std430, binding = 1) buffer IndicesBuffer { uint indices[]; };
layout (std430, binding = 2) buffer OneBuffer { uint pBits[]; };
layout (std430, binding = 3) buffer ZeroBuffer { uint nBits[]; };


uniform uint arraySize;
uniform uint bitMask;


void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= arraySize) return;

	uint maskResult = mortonCodes[indices[index]] & bitMask;			
	pBits[index] = uint(step(1, maskResult));					// If maskResult < 1 => 0.0f, otherwise it's 1.0f
	nBits[index] = pBits[index] ^ 1;
}