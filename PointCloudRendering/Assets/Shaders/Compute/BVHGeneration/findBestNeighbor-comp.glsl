#version 450

#define INT_MAX 0xFFFFFFF

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout (std430, binding = 0) buffer ClusterBuffer { BVHCluster cluster[]; };
layout (std430, binding = 1) buffer NeighborBuffer{ uint neighbor[]; };


uniform uint arraySize;
uniform uint radius;


// ********* FUNCTIONS ************

float surfaceAreaAABB(const uint index1, const uint index2)
{
	const vec3 minPoint = min(cluster[index1].minPoint, cluster[index2].minPoint);
	const vec3 maxPoint = max(cluster[index1].maxPoint, cluster[index2].maxPoint);

	const vec3 length = maxPoint - minPoint;

	return 2.0f * length.x * length.y + 2.0f * length.z * length.y + 2.0f * length.x * length.z;

	//const vec3 minPoint = min(cluster[index1].minPoint, cluster[index2].minPoint);
	//const vec3 maxPoint = max(cluster[index1].maxPoint, cluster[index2].maxPoint);
	//const vec3 p1 = (cluster[index1].minPoint + cluster[index1].maxPoint) / 2.0f;
	//const vec3 p2 = (cluster[index2].minPoint + cluster[index2].maxPoint) / 2.0f;
	//const float distance = distance(p1, p2);
	//const float length = length(maxPoint - minPoint);

	//return distance + length;
}


void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= arraySize) return;

	const uint lowerIndex = max(int(index - radius), 0), upperIndex = min(index + radius, arraySize - 1);
	uint currentIndex = lowerIndex;
	float distance, minDistance = INT_MAX;

	while (currentIndex < index)
	{
		distance = surfaceAreaAABB(index, currentIndex);
		if (distance < minDistance)
		{
			neighbor[index] = currentIndex;
			minDistance = distance;
		}

		++currentIndex;
	}

	++currentIndex;

	while (currentIndex <= upperIndex)
	{
		distance = surfaceAreaAABB(index, currentIndex);
		if (distance < minDistance)
		{
			neighbor[index] = currentIndex;
			minDistance = distance;
		}

		++currentIndex;
	}
}