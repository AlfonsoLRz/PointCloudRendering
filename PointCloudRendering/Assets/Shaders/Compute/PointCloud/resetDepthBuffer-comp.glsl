#version 450

#extension GL_ARB_compute_variable_group_size: enable
#extension GL_ARB_gpu_shader_int64: require

layout (local_size_variable) in;

layout (std430, binding = 0) buffer DepthBuffer { uint64_t depthBuffer[]; };

uniform uvec2 windowSize;

void main()
{
	uint index = gl_GlobalInvocationID.x;
	if (index >= windowSize.x * windowSize.y) return;

	// Maximum value of 64 bits
	depthBuffer[index] = 0;
	depthBuffer[index] = ~depthBuffer[index];
}