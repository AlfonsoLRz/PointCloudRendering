#version 450

#define INT_MAX 0xFFFFFFF

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout (std430, binding = 0) buffer TempClusterBuffer		{ BVHCluster tempCluster[]; };
layout (std430, binding = 1) buffer ClusterBuffer			{ BVHCluster cluster[]; };
layout (std430, binding = 2) buffer NeighborBuffer			{ uint neighbor[]; };
layout (std430, binding = 3) buffer ValidClusterBuffer		{ uint validCluster[]; };
layout (std430, binding = 4) buffer MergedClusterBuffer		{ uint mergedCluster[]; };
layout (std430, binding = 5) buffer PrefixScanBuffer		{ uint prefixScan[]; };
layout (std430, binding = 6) buffer CurrentPositionBuffer	{ uint currentPosition[]; };
layout (std430, binding = 7) buffer NumNodesBuffer			{ uint numNodes; };


uniform uint arraySize;


// ********* FUNCTIONS ************

void buildCluster(const uint index1, const uint index2)
{
	// Thread which manages index1 must call this method, then index1 is treated as the min index

	tempCluster[index1].minPoint	= min(tempCluster[index1].minPoint, tempCluster[index2].minPoint);
	tempCluster[index1].maxPoint	= max(tempCluster[index1].maxPoint, tempCluster[index2].maxPoint);
	tempCluster[index1].prevIndex1	= currentPosition[index1];
	tempCluster[index1].prevIndex2	= currentPosition[index2];
	tempCluster[index1].faceIndex	= INT_MAX;

	// Compact buffer operation
	uint newPosition = atomicAdd(numNodes, 1);
	cluster[newPosition] = tempCluster[index1];
	currentPosition[index1] = newPosition;
}


void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= arraySize) return;

	if (neighbor[neighbor[index]] == index)
	{
		if (index < neighbor[index])
		{
			buildCluster(index, neighbor[index]);
			validCluster[index] = prefixScan[index] = 1;
			validCluster[neighbor[index]] = prefixScan[neighbor[index]] = 0;
			mergedCluster[index] = 1;
		}
		else
		{
			mergedCluster[index] = 0;
			return;
		}
	}
	else
	{
		validCluster[index] = prefixScan[index] = 1;				// By default it's valid
		mergedCluster[index] = 0;
	}
}