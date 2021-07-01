#version 450

#extension GL_ARB_compute_variable_group_size : enable
#extension GL_ARB_gpu_shader_int64 : require

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout(local_size_variable) in;

layout(std430, binding = 0) buffer Color01Buffer { uint64_t		colorBuffer01[]; };
layout(std430, binding = 1) buffer Color02Buffer { uint64_t		colorBuffer02[]; };
uniform layout(rgba8) writeonly image2D texImage;

uniform vec3	backgroundColor;
uniform uvec2	windowSize;

void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= windowSize.x * windowSize.y) return;

	const uint py = uint(floor(index / windowSize.x));
	const uint px = index % windowSize.x;
	const uint64_t rg = colorBuffer01[index];
	const uint64_t ba = colorBuffer02[index];

	const uint a = uint(ba);
	const uint r = uint((rg >> 32) / a);
	const uint g = uint(rg) / a;
	const uint b = uint((ba >> 32) / a);

	vec3 rgbColor = backgroundColor;
	if (a > 0)
	{
		rgbColor = vec3(r, g, b) / 255.0f;
	}

	imageStore(texImage, ivec2(px, py), vec4(rgbColor, 1.0f));
}