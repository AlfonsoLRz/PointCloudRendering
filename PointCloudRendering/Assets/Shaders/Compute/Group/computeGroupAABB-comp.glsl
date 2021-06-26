#version 450

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout (std430, binding = 0) buffer InputBuffer { FaceGPUData faceData[]; };
layout (std430, binding = 1) buffer MaxPoint	{ vec4 maxPoint; };
layout (std430, binding = 2) buffer MinPoint	{ vec4 minPoint; };


uniform uint arraySize;
uniform uint iteration;
uniform uint numThreads;


// ********* FUNCTIONS ************

void mergeAABB(const uint index1, const uint index2)
{
	faceData[index2].minPoint = min(faceData[index1].minPoint, faceData[index2].minPoint);
	faceData[index2].maxPoint = max(faceData[index1].maxPoint, faceData[index2].maxPoint);
}


void main()
{
	const uint threadID = gl_GlobalInvocationID.x;
	if (threadID >= numThreads) return;

	const uint powCurrentIt = uint(pow(2, iteration));
	const uint powNextIt = uint(pow(2, iteration + 1));
	const uint index1 = threadID * powNextIt + powCurrentIt - 1;
	const uint index2 = clamp(threadID * powNextIt + powNextIt - 1, 0, arraySize - 1);
	if (index1 == index2) return;

	mergeAABB(index1, index2);
	
	maxPoint.xyz = faceData[index2].maxPoint * uint(numThreads == 1);
	minPoint.xyz = faceData[index2].minPoint * uint(numThreads == 1);
}