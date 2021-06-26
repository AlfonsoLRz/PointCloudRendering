#version 450

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout (std430, binding = 0) buffer ModelOutput { VertexGPUData vertexData[]; };
layout (std430, binding = 1) buffer RawMeshOutput { uint rawMeshData[]; };

/// Geometry
uniform mat4 mModel;
uniform vec2 gridSize;							// Number of tiles for each dimension
uniform vec2 gridDim_2;							// Plane width and depth
uniform vec2 tileDim;
uniform vec2 maxTextCoord;						// Maximum texture coordinate for each axis so we can repeat or build whatever pattern

/// Topology
uniform uint restartPrimitiveIndex;

// ------------- Vertex displacement ---------------
subroutine vec3 displacementType(const vec3 position, const vec2 textCoord);
subroutine uniform displacementType displacementUniform;

uniform float displacementLength;
uniform sampler2D texDisplacementSampler;


// ********* FUNCTIONS ************

subroutine(displacementType)
vec3 displacementMapping(const vec3 position, const vec2 textCoord)
{
	const float displacement = texture(texDisplacementSampler, textCoord).r;
	const vec3 posAux = position + vec3(0.0f, 1.0f, 0.0f) * displacementLength * displacement;

	return posAux;
}

subroutine(displacementType)
vec3 noDisplacement(const vec3 position, const vec2 textCoord)
{
	return position;
}


void main()
{
	const uvec2 index = gl_GlobalInvocationID.xy;
	if (index.x > gridSize.x || index.y > gridSize.y)
	{
		return;
	}

	// Vertex data
	const uint globalID = uint((1 + gridSize.x) * index.y + index.x);

	vertexData[globalID].position = vec3(index.x * tileDim.x - gridDim_2.x, 0.0f, index.y * tileDim.y - gridDim_2.y);
	vertexData[globalID].normal = vec3(mModel * vec4(0.0f, 1.0f, 0.0f, 0.0f));
	vertexData[globalID].tangent = vec3(mModel * vec4(1.0f, 0.0f, 0.0f, 0.0f));
	vertexData[globalID].textCoord = vec2(index.x / gridSize.x * maxTextCoord.x, maxTextCoord.y - index.y / gridSize.y * maxTextCoord.y);
	vertexData[globalID].position = vec3(mModel * vec4(displacementUniform(vertexData[globalID].position, vertexData[globalID].textCoord), 1.0f));

	// For each tile => 2 triangles
	const uint index1 = uint(index.y * (gridSize.x + 1) + index.x);
	const uint index2 = uint(index.y * (gridSize.x + 1) + index.x + 1);

	// Triangle strip topology
	const uint rawMeshGlobalID = uint(index.x * ((gridSize.y + 1) * 2 + 1) + index.y * 2);

	rawMeshData[rawMeshGlobalID] = index2;
	rawMeshData[rawMeshGlobalID + 1] = index1;

	// Last tile => we need to also define the last two positions
	if (index.y == (gridSize.y - 1))
	{
		const uint index3 = uint((index.y + 1) * (gridSize.x + 1) + index.x);
		const uint index4 = uint((index.y + 1) * (gridSize.x + 1) + index.x + 1);

		rawMeshData[rawMeshGlobalID + 2] = index4;
		rawMeshData[rawMeshGlobalID + 3] = index3;
		rawMeshData[rawMeshGlobalID + 4] = restartPrimitiveIndex;
	}
}