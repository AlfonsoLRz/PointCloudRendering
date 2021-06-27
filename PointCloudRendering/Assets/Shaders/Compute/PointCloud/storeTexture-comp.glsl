#version 450

#extension GL_ARB_compute_variable_group_size: enable
#extension GL_ARB_gpu_shader_int64: require

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout (local_size_variable) in;

layout (std430, binding = 0) buffer DepthBuffer { uint64_t		depthBuffer[]; };
uniform layout (rgba8) writeonly image2D texImage;

uniform vec3	backgroundColor;
uniform uvec2	windowSize;

void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= windowSize.x * windowSize.y) return;

	const uint py		= uint(floor(index / windowSize.x));
	const uint px		= index % windowSize.x;
	const uint colorInd	= uint(depthBuffer[index] & 0x00000000ffffffff);

	vec3 rgbColor = backgroundColor;
	if (colorInd != 0xffffffff)
	{
		rgbColor = unpackUnorm4x8(colorInd).rgb;
	}

	imageStore(texImage, ivec2(px, py), vec4(rgbColor, 1.0f));
}