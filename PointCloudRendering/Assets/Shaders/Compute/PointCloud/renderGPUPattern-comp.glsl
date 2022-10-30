#version 450

#extension GL_ARB_compute_variable_group_size : enable
#extension GL_ARB_gpu_shader_int64 : require
#extension GL_NV_shader_thread_group : require

layout(local_size_variable) in;

layout(std430, binding = 0) buffer Color01Buffer { float smId[]; };

uniform uvec2	windowSize;

void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= windowSize.x * windowSize.y) return;

	const uint py = uint(floor(index / windowSize.x));
	const uint px = index % windowSize.x;

	smId[px * windowSize.x + py] = gl_SMIDNV;
}