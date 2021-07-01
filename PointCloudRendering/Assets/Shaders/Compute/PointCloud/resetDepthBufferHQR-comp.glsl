#version 450

#extension GL_ARB_compute_variable_group_size: enable
#extension GL_NV_gpu_shader5: enable

layout (local_size_variable) in;

layout (std430, binding = 0) buffer DepthBuffer		{ uint			depthBuffer[]; };
layout (std430, binding = 1) buffer Color01Buffer	{ uint64_t		colorBuffer01[]; };
layout (std430, binding = 2) buffer Color02Buffer	{ uint64_t		colorBuffer02[]; };

uniform uvec2 windowSize;

void main()
{
	uint index = gl_GlobalInvocationID.x;
	if (index >= windowSize.x * windowSize.y) return;

	depthBuffer[index] = 0;
	depthBuffer[index] = ~depthBuffer[index];

	colorBuffer01[index] = colorBuffer02[index] = 0;
}