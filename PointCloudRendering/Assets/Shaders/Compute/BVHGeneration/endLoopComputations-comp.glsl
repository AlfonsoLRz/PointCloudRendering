#version 450

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

layout (std430, binding = 0) buffer ArraySizeBuffer { uint arraySizeCount; };
layout (std430, binding = 1) buffer PrefixScanBuffer { uint prefixScan[]; };
layout (std430, binding = 2) buffer ValidClusterBuffer { uint validCluster[]; };


uniform uint arraySize;


void main()
{
	arraySizeCount = prefixScan[arraySize - 1] + validCluster[arraySize - 1];
}