#version 450

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout (std430, binding = 0) buffer InClusterBuffer { BVHCluster inCluster[]; };
layout (std430, binding = 1) buffer OutClusterBuffer { BVHCluster outCluster[]; };
layout (std430, binding = 2) buffer ValidClusterBuffer { uint validCluster[]; };
layout (std430, binding = 3) buffer PrefixScanBuffer { uint prefixScan[]; };
layout (std430, binding = 4) buffer InGlobalPositionBuffer { uint inPosition[]; };
layout (std430, binding = 5) buffer OutGlobalPositionBuffer { uint outPosition[]; };


uniform uint arraySize;


void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= arraySize)
	{
		return;
	}

	if (validCluster[index] == 1)
	{
		outCluster[prefixScan[index]] = inCluster[index];
		outPosition[prefixScan[index]] = inPosition[index];
	}
}