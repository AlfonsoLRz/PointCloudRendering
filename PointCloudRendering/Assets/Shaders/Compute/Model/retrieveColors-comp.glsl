#version 450

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout (std430, binding = 0) buffer InputBufferA	{ VertexGPUData vertexData[]; };

uniform uint		size;
uniform sampler2D	texKadSampler;
uniform sampler2D	texKsSampler;

void main()
{
	uint index = gl_GlobalInvocationID.x;
	if (index >= size) return;

	vertexData[index].kad	= texture(texKadSampler, vertexData[index].textCoord).xyz;
	vertexData[index].ks	= texture(texKsSampler, vertexData[index].textCoord).r;
}