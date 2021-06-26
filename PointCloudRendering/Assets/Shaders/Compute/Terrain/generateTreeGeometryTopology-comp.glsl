#version 450

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout (std430, binding = 0) buffer GeometryBuffer			{ VertexGPUData geometry[]; };
layout (std430, binding = 1) buffer FaceBuffer				{ FaceGPUData face[]; };
layout (std430, binding = 2) buffer ModelGeometryBuffer		{ VertexGPUData modelGeometry[]; };
layout (std430, binding = 3) buffer ModelFaceBuffer			{ FaceGPUData modelFace[]; };
layout (std430, binding = 4) buffer PositionBuffer			{ vec4 translation[]; };
layout (std430, binding = 5) buffer RotationBuffer			{ vec4 rotation[]; };
layout (std430, binding = 6) buffer ScaleBuffer				{ vec4 scale[]; };

#define PI 3.1415926535f

uniform uint	numTrees;
uniform uint	restartPrimitiveIndex;

// Data of reference model
uniform uint	modelGeomSize, modelFaceSize;

// Vectors offset
uniform uint	globalOffset;

void computeAABB(const uint indexFace)
{
	const vec3 a = geometry[face[indexFace].vertices.x].position;
	const vec3 b = geometry[face[indexFace].vertices.y].position;
	const vec3 c = geometry[face[indexFace].vertices.z].position;

	face[indexFace].minPoint = min(a, min(b, c));
	face[indexFace].maxPoint = max(a, max(b, c));
}


void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= numTrees)
	{
		return;
	}

	mat4 scaleMatrix = mat4(scale[index].x, .0f, .0f, .0f, .0f, scale[index].y, .0f, .0f, .0f, .0f, scale[index].z, .0f, .0f, .0f, .0f, 1.0f);
	mat4 translationMatrix = mat4(1.0f, .0f, .0f, .0f, .0f, 1.0f, .0f, .0f, .0f, .0f, 1.0f, .0f, translation[index].x, translation[index].y, translation[index].z, 1.0f);
	mat4 rotationXMatrix = mat4(1.0f, .0f, .0f, .0f, .0f, cos(rotation[index].x), sin(rotation[index].x), .0f, .0f, -sin(rotation[index].x), cos(rotation[index].x), .0f, .0f, .0f, .0f, 1.0f);
	mat4 rotationYMatrix = mat4(cos(rotation[index].y), .0f, -sin(rotation[index].y), .0f, .0f, 1.0f, .0f, .0f, sin(rotation[index].y), .0f, cos(rotation[index].y), .0f, .0f, .0f, .0f, 1.0f);
	mat4 rotationZMatrix = mat4(cos(rotation[index].z), sin(rotation[index].z), .0f, .0f, -sin(rotation[index].z), cos(rotation[index].z), .0f, .0f, .0f, .0f, 1.0f, .0f, .0f, .0f, .0f, 1.0f);
	mat4 finalMatrix = translationMatrix * scaleMatrix * rotationZMatrix * rotationYMatrix * rotationXMatrix;

	uint geomBase = index * modelGeomSize;
	for (int geom = 0; geom < modelGeomSize; ++geom)
	{
		vec3 position = vec3(finalMatrix * vec4(modelGeometry[geom].position, 1.0f));
		vec3 normal = normalize(vec3(finalMatrix * vec4(modelGeometry[geom].normal, .0f)));
		vec3 tangent = normalize(vec3(finalMatrix * vec4(modelGeometry[geom].tangent, .0f)));

		geometry[geomBase + geom].position = position;
		geometry[geomBase + geom].normal = normal;
		geometry[geomBase + geom].tangent = tangent;
		geometry[geomBase + geom].textCoord = modelGeometry[geom].textCoord;
	}

	uint faceBase = index * modelFaceSize;
	for (int i = 0; i < modelFaceSize; ++i)
	{
		FaceGPUData faceData = modelFace[i];
		faceData.vertices += geomBase;
		faceData.normal = normalize(vec3(finalMatrix * vec4(faceData.normal, .0f)));
		face[faceBase + i] = faceData;

		computeAABB(faceBase + i);

		face[faceBase + i].vertices += globalOffset;
	}
}