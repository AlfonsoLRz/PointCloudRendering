#version 450

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout (std430, binding = 0) buffer ModelInput { VertexGPUData vertexData[]; };
layout (std430, binding = 1) buffer MeshOutput	{ FaceGPUData meshData[]; };

#include <Assets/Shaders/Compute/Model/computeNormals-comp.glsl>			// Needs input buffers

/// Geometry
uniform uvec2 gridSize;							// Number of tiles for each dimension
uniform uint numFaces;							// Triangle array size


/// Others

void computeAABB(const uint indexFace)
{
	const vec3 a = vertexData[meshData[indexFace].vertices.x].position;
	const vec3 b = vertexData[meshData[indexFace].vertices.y].position;
	const vec3 c = vertexData[meshData[indexFace].vertices.z].position;

	meshData[indexFace].minPoint = min(a, min(b, c));
	meshData[indexFace].maxPoint = max(a, max(b, c));
}


void main()
{
	const uint threadIndex = gl_GlobalInvocationID.x;
	const uint index = threadIndex * 2;						// Each thread computes two faces
	if (index >= numFaces)
	{
		return;
	}

	const uint row = uint(floor(threadIndex / gridSize.y));
	const uint column = threadIndex % gridSize.x;

	// For each tile => 2 triangles
	const uint index1 = uint(row * (gridSize.x + 1) + column);
	const uint index2 = uint(row * (gridSize.x + 1) + column + 1);
	const uint index3 = uint((row + 1) * (gridSize.x + 1) + column);
	const uint index4 = uint((row + 1) * (gridSize.x + 1) + column + 1);

	meshData[index].vertices = uvec3(index1, index4, index2);
	meshData[index + 1].vertices = uvec3(index1, index3, index4);
	meshData[index].normal = computeNormal(index);
	meshData[index + 1].normal = computeNormal(index + 1);

	computeAABB(index);						// Axis aligned bounding box for the face
	computeAABB(index + 1);		
}