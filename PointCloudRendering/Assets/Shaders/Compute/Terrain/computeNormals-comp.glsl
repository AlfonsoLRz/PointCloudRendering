#version 450

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

#define NORMAL_MULT 1000.0f

layout (std430, binding = 0) buffer ModelInput  { VertexGPUData vertexData[]; };
layout (std430, binding = 1) buffer NormalCalc  { ivec4 normalData[]; };
layout (std430, binding = 2) buffer NumNormals  { ivec4 numNormals[]; };

uniform uint numVertices;


void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= numVertices)
	{
		return;
	}

	vertexData[index].normal = normalize(vec3(normalData[index] / NORMAL_MULT / float(numNormals[index].x)));
}