#version 450

#extension GL_ARB_compute_variable_group_size : enable
#extension GL_ARB_gpu_shader_int64 : require
#extension GL_NV_shader_thread_group : require

#define PHI 0.618033988749895f

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

uniform layout(rgba8) writeonly image2D texImage;

uniform sampler2D textureImage;
uniform uvec2	windowSize;

float getHueValue(uint id)
{
	float h = id * PHI;

	return mod(h, 1.0f);
}

vec3 HSVtoRGB(float h, float s, float v)
{
	const int h_i = int(h * 6.0f);
	const float f = h * 6.0f - h_i;
	const float p = v * (1.0f - s);
	const float q = v * (1.0f - f * s);
	const float t = v * (1.0f - (1.0f - f) * s);

	switch (h_i)
	{
	case 0:
		return vec3(v, t, p);
	case 1:
		return vec3(q, v, p);
	case 2:
		return vec3(p, v, t);
	case 3:
		return vec3(p, q, v);
	case 4:
		return vec3(t, p, v);
	case 5:
		return vec3(v, p, q);
	}

	return vec3(.0f);
}


void main()
{
	const uvec2 index = gl_GlobalInvocationID.xy;
	if (index.x >= windowSize.x || index.y > windowSize.y) return;

	vec3 normalizedSM = texture(textureImage, vec2(.5f, gl_SMIDNV / float(gl_SMCountNV - 1))).rgb;
	//vec3 normalizedSM = texture(textureImage, vec2(.5f, gl_WarpIDNV / float(gl_WarpsPerSMNV - 1))).rgb;
	//vec3 normalizedSM = texture(textureImage, vec2(.5f, gl_ThreadInWarpNV / float(gl_WarpSizeNV - 1))).rgb;
	imageStore(texImage, ivec2(index), vec4(normalizedSM, 1.0f));
}