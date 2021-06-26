#version 450

#define VEC3_UVEC3_FACTOR 1000.0f

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout (std430, binding = 0) buffer InputBufferA { VertexGPUData vertexData[]; };
layout (std430, binding = 1) buffer InputBufferB { FaceGPUData faceData[]; };
layout (std430, binding = 2) buffer OutputBufferA { uvec4 vertexTangent[]; };					// Tangent (vec3) + tangent counter


uniform int numTriangles;						// Groups x threads per groups gives more than required triangles


void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= numTriangles)
	{
		return;
	}

	const uint index1 = faceData[index].vertices.x, index2 = faceData[index].vertices.y, index3 = faceData[index].vertices.z;

	const vec3 v1 = vertexData[index1].position, v2 = vertexData[index2].position, v3 = vertexData[index3].position;
	const vec2 w1 = vertexData[index1].textCoord, w2 = vertexData[index2].textCoord, w3 = vertexData[index3].textCoord;

	const float x1 = v2.x - v1.x, x2 = v3.x - v1.x;
	const float y1 = v2.y - v1.y, y2 = v3.y - v1.y;
	const float z1 = v2.z - v1.z, z2 = v3.z - v1.z;

	const float s1 = w2.x - w1.x, s2 = w3.x - w1.x;
	const float t1 = w2.y - w1.y, t2 = w3.y - w1.y;

	const float r = 1.0f / (s1 * t2 - s2 * t1);
	const vec3 sdir = vec3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
	const uvec3 udir = uvec3((sdir + VEC3_UVEC3_FACTOR) * VEC3_UVEC3_FACTOR);

	atomicAdd(vertexTangent[index1].x, udir.x);
	atomicAdd(vertexTangent[index1].y, udir.y);
	atomicAdd(vertexTangent[index1].z, udir.z);
	atomicAdd(vertexTangent[index1].w, 1);							// We need to count the number of tangents to undo the transformation

	atomicAdd(vertexTangent[index2].x, udir.x);
	atomicAdd(vertexTangent[index2].y, udir.y);
	atomicAdd(vertexTangent[index2].z, udir.z);
	atomicAdd(vertexTangent[index2].w, 1);

	atomicAdd(vertexTangent[index3].x, udir.x);
	atomicAdd(vertexTangent[index3].y, udir.y);
	atomicAdd(vertexTangent[index3].z, udir.z);
	atomicAdd(vertexTangent[index3].w, 1);
}