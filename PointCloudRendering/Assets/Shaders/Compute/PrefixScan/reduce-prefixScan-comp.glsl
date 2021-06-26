#version 450

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

layout (std430, binding = 0) buffer InputBuffer { uint inBuffer[]; };

uniform uint arraySize;
uniform uint iteration;
uniform uint numThreads;


void main()
{
	const uint threadID = gl_GlobalInvocationID.x;
	if (threadID >= numThreads) return;

	const uint powCurrentIt = uint(pow(2, iteration));
	const uint powNextIt = uint(pow(2, iteration + 1));
	const uint index1 = clamp(threadID * powNextIt + powNextIt - 1, 0, arraySize - 1);
	const uint index2 = clamp(threadID * powNextIt + powCurrentIt - 1, 0, arraySize - 1);
	if (index1 == index2) return;

	inBuffer[index1] = inBuffer[index1] + inBuffer[index2];
}