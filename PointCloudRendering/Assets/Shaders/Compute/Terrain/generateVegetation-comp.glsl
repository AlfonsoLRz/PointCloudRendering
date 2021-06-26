#version 450

#extension GL_ARB_compute_variable_group_size: enable
layout (local_size_variable) in;

#include <Assets/Shaders/Compute/Templates/modelStructs.glsl>

layout (std430, binding = 0) buffer RandomPosBuffer			{ vec4 randomPosData[]; };
layout (std430, binding = 1) buffer VerticesOutput			{ VertexGPUData vertexData[]; };
layout (std430, binding = 2) buffer MeshDataOutput			{ uint rawMeshData[]; };
layout (std430, binding = 3) buffer TriangleMeshDataOutput	{ FaceGPUData faceData[]; };
layout (std430, binding = 4) buffer NumPlantsBuffer			{ uint numPlants; };

#define PI 3.1415926535f

const uint	NUM_VERTICES_GRASS = 7;
const uint	NUM_FACES_GRASS = 5;
const float GRASS_SCALE = 0.03f;				// Uniform scale for grass

// Tall straight grass:
const float GRASS_1_X[7] = float[7](-0.329877f, 0.329877f, -0.212571f, 0.212571f, -0.173286f, 0.173286f, 0.0f);
const float GRASS_1_Y[7] = float[7](0.0f, 0.0f, 2.490297f, 2.490297f, 4.847759f, 4.847759f, 8.0f);

// Tall bent grass:
const float GRASS_2_X[7] = float[7](-1.035275f, 1.035275f, -0.437324f, 1.36106f, 1.64639f, 2.406257f, 3.0f);
const float GRASS_2_Y[7] = float[7](0.0f, 0.0f, 3.691449f, 3.691449f, 7.022911f, 7.022911f, 8.0f);

// Chubby double grass:
const float GRASS_3_X[7] = float[7](-3.0f, -1.0f, -2.0f, 2.4f, 1.2f, 1.6f, 1.9f);
const float GRASS_3_Y[7] = float[7](1.8f, 1.0f, 0.0f, 0.0f, 1.8f, 1.8f, 4.0f);

// Triangle order for each grass type:
const uvec3 GRASS_1_TRIANGLE[5] = uvec3[5](uvec3(0, 1, 2), uvec3(1, 3, 2), uvec3(2, 3, 4), uvec3(3, 5, 4), uvec3(4, 5, 6));
const uvec3 GRASS_2_TRIANGLE[5] = uvec3[5](uvec3(0, 1, 2), uvec3(1, 3, 2), uvec3(2, 3, 4), uvec3(3, 5, 4), uvec3(4, 5, 6));
const uvec3 GRASS_3_TRIANGLE[5] = uvec3[5](uvec3(0, 2, 1), uvec3(1, 2, 3), uvec3(2, 3, 4), uvec3(3, 5, 4), uvec3(4, 5, 6));

// u,v coordinates for grass locations inside triangle
const float COORDS_U[6] = float[6](0.125f, 0.125f, 0.4375f, 0.125f, 0.4375f, 0.75f);
const float COORDS_V[6] = float[6](0.75f, 0.4375f, 0.4375f, 0.125f, 0.125f, 0.125f);

// Vegetation
uniform float		minScale, maxScale;

// Terrain
uniform sampler2D	heightMap;
uniform sampler2D	normalMap;
uniform sampler2D	vegetationMap;
uniform float		vegetationThreshold;

/// Topology
uniform uint		numTriangles;
uniform uint		restartPrimitiveIndex;

// Parameters to compute position
uniform float		width, depth;
uniform mat4		modelMatrix;
uniform float		terrainDisp;

// Random number
float rand(const vec2 co)
{
	return fract(sin(dot(co, vec2(12.9898f, 78.233f))) * 43758.5453f);
}

// Computes the AABB for a triangle
void computeAABB(const uint indexFace)
{
	const vec3 a = vertexData[faceData[indexFace].vertices.x].position;
	const vec3 b = vertexData[faceData[indexFace].vertices.y].position;
	const vec3 c = vertexData[faceData[indexFace].vertices.z].position;

	faceData[indexFace].minPoint = min(a, min(b, c));
	faceData[indexFace].maxPoint = max(a, max(b, c));
}

void generateGrass(const uint index, const vec3 pos, const vec3 normal, const vec2 textCoord, const float[7] grassX, const float[7] grassY, const uvec3[5] triangleIndices)
{
	const uint basePosIndex_01 = index * NUM_VERTICES_GRASS, basePosIndex_02 = index * (NUM_VERTICES_GRASS + 1), baseFaceIndex = index * NUM_FACES_GRASS;
	const float rad = rand(pos.xy) * 2.0f * PI;
	const mat4 yRotation = mat4(cos(rad), .0f, -sin(rad), .0f, .0f, 1.0f, .0f, .0f, sin(rad), .0f, cos(rad), .0f, .0f, .0f, .0f, 1.0f);
	vec3 gradient = vec3(rand(pos.xy) * .8f, .7f, rand(pos.xz) * 0.8f);
	gradient -= -0.8f * vec3(.0f, 1.0f, .0f) * dot(vec3(.0f, 1.0f, .0f), gradient);							// Project onto xz-plane

	float bendInterpolation;
	const vec3 instanceNormal = normalize((yRotation * vec4(.0f, .0f, 1.0f, .0f)).xyz);
	const vec3 instanceTangent = normalize((yRotation * vec4(normalize(cross(vec3(.0f, .0f, -1.0f), normal)), .0f)).xyz);
	const float scale = rand(textCoord) * (maxScale - minScale) + minScale;

	for (int i = 0; i < NUM_VERTICES_GRASS; i++) 
	{
		vec3 y = scale * normal * grassY[i];
		vec3 xz = vec3(yRotation * vec4(GRASS_SCALE * (grassX[i] * gradient), 1.0f));

		const vec3 instancePos = pos + xz + y;

		vertexData[basePosIndex_01 + i].position = instancePos;
		vertexData[basePosIndex_01 + i].normal = abs(instanceNormal);
		vertexData[basePosIndex_01 + i].textCoord = vec2(.0f);
		vertexData[basePosIndex_01 + i].tangent = instanceTangent;

		rawMeshData[basePosIndex_02 + i] = index * NUM_VERTICES_GRASS + i;
	}

	rawMeshData[basePosIndex_02 + NUM_VERTICES_GRASS] = restartPrimitiveIndex;

	for (int i = 0; i < NUM_FACES_GRASS; ++i)
	{
		faceData[baseFaceIndex + i].vertices = uvec3(7 * index) + triangleIndices[i];
		faceData[baseFaceIndex + i].normal = abs(instanceNormal);
		computeAABB(baseFaceIndex + i);
	}
}

void main()
{
	const uint index = gl_GlobalInvocationID.x;
	if (index >= numTriangles)
	{
		return;
	}

	vec2 randomPos = randomPosData[index].xy;
	vec2 textCoord = randomPos;
	float probability = texture(vegetationMap, textCoord).r;

	if (probability > vegetationThreshold)
	{	
		uint plantID = atomicAdd(numPlants, 1);
		vec3 normal = vec3((2.0f * texture(normalMap, textCoord)) - 1.0f);
		normal = vec3(normal.x, normal.z, normal.y);
		vec4 plantPosition = vec4(randomPos.x * width - width / 2.0f, .0f, (1.0f - randomPos.y) * depth - depth / 2.0f, 1.0f);
		plantPosition = modelMatrix * plantPosition;
		plantPosition = plantPosition + vec4(.0, 1.0f, .0f, .0f) * texture(heightMap, randomPos).r * terrainDisp;;

		generateGrass(plantID, plantPosition.xyz, normal, textCoord, GRASS_2_X, GRASS_2_Y, GRASS_2_TRIANGLE);
	}
}