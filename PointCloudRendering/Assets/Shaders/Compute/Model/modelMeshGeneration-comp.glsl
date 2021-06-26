#version 450

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout (std430, binding = 0) buffer InputBufferA	{ VertexGPUData vertexData[]; };
layout (std430, binding = 1) buffer OutputBufferA	{ FaceGPUData meshData[]; };
layout (std430, binding = 2) buffer OutputBufferB	{ uint outBuffer[]; };

#include <Assets/Shaders/Compute/Model/computeNormals-comp.glsl>			// Needs input buffers

uniform int size;
uniform uint restartPrimitiveIndex;


// ********* FUNCTIONS ************

void computeAABB(const uint indexFace)
{
	const vec3 a = vertexData[meshData[indexFace].vertices.x].position;
	const vec3 b = vertexData[meshData[indexFace].vertices.y].position;
	const vec3 c = vertexData[meshData[indexFace].vertices.z].position;

	meshData[indexFace].minPoint = min(a, min(b, c));
	meshData[indexFace].maxPoint = max(a, max(b, c));
}


void main()
{
	uint index = gl_GlobalInvocationID.x;

	if (index >= size)
	{
		return;
	}

	uint outputBufferIndex = index * 4;
	outBuffer[outputBufferIndex] = meshData[index].vertices.x;
	outBuffer[outputBufferIndex + 1] = meshData[index].vertices.y;
	outBuffer[outputBufferIndex + 2] = meshData[index].vertices.z;
	outBuffer[outputBufferIndex + 3] = restartPrimitiveIndex;

	// Boundaries and location data for BVH
	computeAABB(index);
	meshData[index].normal = computeNormal(index);
}