#version 450

#define VEC3_UVEC3_FACTOR 1000.0f

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout (std430, binding = 0) buffer InputBufferA { VertexGPUData vertexData[]; };
layout (std430, binding = 1) buffer InputBufferB { uvec4 vertexTangent[]; };


uniform int numVertices;


void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= numVertices)
	{
		return;
	}

	const vec3 n = vertexData[index].normal;
	const vec3 t = vec3(vertexTangent[index].xyz / VEC3_UVEC3_FACTOR) - VEC3_UVEC3_FACTOR * vertexTangent[index].w;		// Undo summatory by multiplying for tangent counter
	vertexData[index].tangent = normalize(t - n * dot(n, t));															// Gram-Schmidt orthogonalize
}