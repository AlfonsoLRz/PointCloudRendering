#version 450

#extension GL_ARB_compute_variable_group_size: enable
#extension GL_ARB_gpu_shader_int64: require

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout (local_size_variable) in;

//layout (std430, binding = 0) buffer DepthBuffer { uint64_t		depthBuffer[]; };
//layout (std430, binding = 1) buffer PointBuffer { PointModel	points[]; };
uniform layout (rgba8) writeonly image2D texImage;

uniform uvec2	windowSize;

void main()
{
	const uint index = gl_GlobalInvocationID.x;

	const uint py = uint(floor(index / windowSize.x));
	const uint px = index % windowSize.x;

	imageStore(texImage, ivec2(px, py), vec4(1.0f));
}