#version 450

#define INT_MAX 0xFFFFFFF

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout (std430, binding = 0) buffer FaceBuffer { FaceGPUData faceBuffer[]; };
layout (std430, binding = 1) buffer SortedIndices { uint indices[]; };
layout (std430, binding = 2) buffer ClusterBuffer { BVHCluster cluster[]; };
layout (std430, binding = 3) buffer TempClusterBuffer { BVHCluster tempCluster[]; };


uniform uint arraySize;


void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= arraySize) return;

	// indices buffer (morton codes sorted) references a faces different than index
	const uint defIndex = indices[index];

	cluster[index].minPoint = faceBuffer[defIndex].minPoint;
	cluster[index].maxPoint = faceBuffer[defIndex].maxPoint;
	cluster[index].prevIndex1 = cluster[index].prevIndex2 = INT_MAX;
	cluster[index].faceIndex = defIndex;
	tempCluster[index] = cluster[index];
}