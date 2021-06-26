#version 450

// ********** PARAMETERS & VARIABLES ***********

in vec2 textCoord;

uniform sampler2D texPosition;
uniform sampler2D texNormal;
uniform sampler2D texKernels;
uniform sampler2D texNoise;

uniform mat4 mProjMatrix;
uniform vec2 windowSize;

#define BIAS 0.01f
#define KERNEL_SIZE 64
#define NOISE_SCALE 4.0f
#define RADIUS 0.2f

#define MIN_OCCLUSION 0.3f

layout (location = 0) out vec4 fColor;

void main(void)
{
	vec3 fragPos   = texture(texPosition, textCoord).xyz;
	vec3 normal    = normalize(texture(texNormal, textCoord).rgb);
	vec3 randomVec = normalize(texture(texNoise, textCoord * windowSize / NOISE_SCALE).xyz);

	vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = normalize(cross(normal, tangent));
	mat3 TBN       = mat3(tangent, bitangent, normal); 

	float occlusion = 0.0;
	for(int i = 0; i < KERNEL_SIZE; ++i)
	{
		vec3 currentSample = texture(texKernels, vec2(float(i) / 64.0f + 0.5f / 64.0f, .5f)).xyz;
		vec3 samplePos = TBN * currentSample;			// From tangent to view-space
		samplePos = fragPos + samplePos * RADIUS;		// Sample position
    
		vec4 offset = vec4(samplePos, 1.0f);
		offset      = mProjMatrix * offset;				// From view to clip-space
		offset.xyz /= offset.w;							// Perspective division
		offset.xyz  = offset.xyz * 0.5f + 0.5f;			// To range [0, 1]
	
		float sampleDepth = texture(texPosition, offset.xy).z; 

		float rangeCheck = smoothstep(0.0f, 1.0f, RADIUS / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + BIAS ? 1.0 : 0.0) * rangeCheck;
	}  

	occlusion = (1.0 - (occlusion / KERNEL_SIZE)) * (1.0f - MIN_OCCLUSION) + MIN_OCCLUSION;

	fColor = vec4(vec3(occlusion), 1.0f);
}
