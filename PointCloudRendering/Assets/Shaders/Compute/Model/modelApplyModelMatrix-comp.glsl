#version 450

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout (std430, binding = 0) buffer InputBuffer { VertexGPUData vertexData[]; };

uniform mat4 mModel;
uniform int size;

// ------------- Vertex displacement ---------------
subroutine vec3 displacementType(uint index);
subroutine uniform displacementType displacementUniform;

uniform float displacementLength;
uniform sampler2D texDisplacementSampler;


// ********* FUNCTIONS ************

subroutine(displacementType)
vec3 displacementMapping(uint index)
{
	const float displacement = texture(texDisplacementSampler, vertexData[index].textCoord).r;
	const vec3 posAux = vertexData[index].position + vertexData[index].normal * displacementLength * displacement;

	return posAux;
}

subroutine(displacementType)
vec3 noDisplacement(uint index)
{
	return vertexData[index].position;
}


void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= size)
	{
		return;
	}

	vertexData[index].position = vec3(mModel * vec4(vertexData[index].position, 1.0f));
	vertexData[index].normal = vec3(mModel * vec4(vertexData[index].normal, 0.0f));
	vertexData[index].position = displacementUniform(index);
}