#include "stdafx.h"
#include "ShaderList.h"

// [Static members initialization]

std::unordered_map<uint8_t, std::string> ShaderList::COMP_SHADER_SOURCE {
		{RendEnum::BIT_MASK_RADIX_SORT, "Assets/Shaders/Compute/RadixSort/bitMask-radixSort"},
		{RendEnum::BUILD_CLUSTER_BUFFER, "Assets/Shaders/Compute/BVHGeneration/buildClusterBuffer"},
		{RendEnum::CLUSTER_MERGING, "Assets/Shaders/Compute/BVHGeneration/clusterMerging"},
		{RendEnum::COMPUTE_FACE_AABB, "Assets/Shaders/Compute/Model/computeFaceAABB"},
		{RendEnum::COMPUTE_GROUP_AABB, "Assets/Shaders/Compute/Group/computeGroupAABB"},
		{RendEnum::COMPUTE_MORTON_CODES, "Assets/Shaders/Compute/BVHGeneration/computeMortonCodes"},
		{RendEnum::COMPUTE_MORTON_CODES_PCL, "Assets/Shaders/Compute/PointCloud/computeMortonCodes"},
		{RendEnum::COMPUTE_TANGENTS_1, "Assets/Shaders/Compute/Model/computeTangents_1"},
		{RendEnum::COMPUTE_TANGENTS_2, "Assets/Shaders/Compute/Model/computeTangents_2"},
		{RendEnum::DOWN_SWEEP_PREFIX_SCAN, "Assets/Shaders/Compute/PrefixScan/downSweep-prefixScan"},
		{RendEnum::END_LOOP_COMPUTATIONS, "Assets/Shaders/Compute/BVHGeneration/endLoopComputations"},
		{RendEnum::FIND_BEST_NEIGHBOR, "Assets/Shaders/Compute/BVHGeneration/findBestNeighbor"},
		{RendEnum::MODEL_APPLY_MODEL_MATRIX, "Assets/Shaders/Compute/Model/modelApplyModelMatrix"},
		{RendEnum::MODEL_MESH_GENERATION, "Assets/Shaders/Compute/Model/modelMeshGeneration"},
		{RendEnum::PLANAR_SURFACE_GENERATION, "Assets/Shaders/Compute/PlanarSurface/planarSurfaceGeometryTopology"},
		{RendEnum::PLANAR_SURFACE_TOPOLOGY, "Assets/Shaders/Compute/PlanarSurface/planarSurfaceFaces"},
		{RendEnum::PROJECTION_SHADER, "Assets/Shaders/Compute/PointCloud/computeDepthBuffer"},
		{RendEnum::REALLOCATE_CLUSTERS, "Assets/Shaders/Compute/BVHGeneration/reallocateClusters"},
		{RendEnum::REALLOCATE_RADIX_SORT, "Assets/Shaders/Compute/RadixSort/reallocateIndices-radixSort"},
		{RendEnum::REDUCE_PREFIX_SCAN, "Assets/Shaders/Compute/PrefixScan/reduce-prefixScan"},
		{RendEnum::RESET_BUFFER_INDEX, "Assets/Shaders/Compute/Generic/resetBufferIndex"},
		{RendEnum::RESET_DEPTH_BUFFER_SHADER, "Assets/Shaders/Compute/PointCloud/resetDepthBuffer"},
		{RendEnum::RESET_LAST_POSITION_PREFIX_SCAN, "Assets/Shaders/Compute/PrefixScan/resetLastPosition-prefixScan"},
		{RendEnum::STORE_TEXTURE_SHADER, "Assets/Shaders/Compute/PointCloud/storeTexture"},
};

std::unordered_map<uint8_t, std::string> ShaderList::REND_SHADER_SOURCE {
		{RendEnum::BLUR_SSAO_SHADER, "Assets/Shaders/2D/blurSSAOShader"},
		{RendEnum::DEBUG_QUAD_SHADER, "Assets/Shaders/Triangles/debugQuad"},
		{RendEnum::POINT_CLOUD_SHADER, "Assets/Shaders/Points/pointCloud"},
		{RendEnum::TRIANGLE_MESH_SHADER, "Assets/Shaders/Triangles/triangleMesh"},
		{RendEnum::TRIANGLE_MESH_GROUP_SHADER, "Assets/Shaders/Triangles/triangleMeshGroup"},
		{RendEnum::TRIANGLE_MESH_NORMAL_SHADER, "Assets/Shaders/Triangles/triangleMeshNormal"},
		{RendEnum::TRIANGLE_MESH_POSITION_SHADER, "Assets/Shaders/Triangles/triangleMeshPosition"},
		{RendEnum::WIREFRAME_SHADER, "Assets/Shaders/Lines/wireframe"},
		{RendEnum::REFLECTIVE_TRIANGLE_MESH_SHADER, "Assets/Shaders/Triangles/reflectiveTriangleMesh"},
		{RendEnum::SHADOWS_SHADER, "Assets/Shaders/Triangles/shadowsShader"},
		{RendEnum::SSAO_SHADER, "Assets/Shaders/2D/ssaoShader"},
};

std::vector<std::unique_ptr<ComputeShader>> ShaderList::_computeShader (RendEnum::numComputeShaderTypes());
std::vector<std::unique_ptr<RenderingShader>> ShaderList::_renderingShader (RendEnum::numRenderingShaderTypes());

/// [Protected methods]

ShaderList::ShaderList()
{
}

/// [Public methods]

ComputeShader* ShaderList::getComputeShader(const RendEnum::CompShaderTypes shader)
{
	const int shaderID = shader;

	if (!_computeShader[shader].get())
	{
		ComputeShader* shader = new ComputeShader();
		shader->createShaderProgram(COMP_SHADER_SOURCE.at(shaderID).c_str());

		_computeShader[shaderID].reset(shader);
	}

	return _computeShader[shaderID].get();
}

RenderingShader* ShaderList::getRenderingShader(const RendEnum::RendShaderTypes shader)
{
	const int shaderID = shader;

	if (!_renderingShader[shader].get())
	{
		RenderingShader* shader = new RenderingShader();
		shader->createShaderProgram(REND_SHADER_SOURCE.at(shaderID).c_str());

		_renderingShader[shaderID].reset(shader);
	}

	return _renderingShader[shader].get();
}
