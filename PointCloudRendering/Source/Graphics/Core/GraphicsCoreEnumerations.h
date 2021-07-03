#pragma once

/**
*	@file GraphicsCoreEnumerations.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/10/2019
*/

/**
*	@brief Contains any enum related to rendering process.
*/
struct RendEnum
{
	/// [Geometry and topology]

	// Topology types
	enum IBOTypes: uint8_t
	{
		IBO_POINT_CLOUD,
		IBO_WIREFRAME,
		IBO_TRIANGLE_MESH
	};

	// Geometry types
	enum VBOTypes : uint8_t
	{
		VBO_POSITION,
		VBO_NORMAL,
		VBO_TEXT_COORD,
		VBO_TANGENT
	};

	/**
	*	@return Number of VBO different types.
	*/
	const static GLsizei numIBOTypes() { return IBO_TRIANGLE_MESH + 1; }

	/**
	*	@return Number of VBO different types.
	*/
	const static GLsizei numVBOTypes() { return VBO_TANGENT + 1; }

	/// [Shaders]

	// Shader implementations
	enum RendShaderTypes : uint8_t
	{
		// General
		POINT_CLOUD_SHADER,
		WIREFRAME_SHADER,
		REFLECTIVE_TRIANGLE_MESH_SHADER,
		TRIANGLE_MESH_SHADER,
		TRIANGLE_MESH_GROUP_SHADER,
		TRIANGLE_MESH_NORMAL_SHADER,
		TRIANGLE_MESH_POSITION_SHADER,
		SHADOWS_SHADER,

		// SSAO
		BLUR_SSAO_SHADER,
		SSAO_SHADER,

		// Debug quad
		DEBUG_QUAD_SHADER,
	};

	enum CompShaderTypes : uint8_t
	{
		// BVH
		BUILD_CLUSTER_BUFFER,
		CLUSTER_MERGING,
		COMPUTE_FACE_AABB,
		COMPUTE_GROUP_AABB,
		COMPUTE_MORTON_CODES,
		DOWN_SWEEP_PREFIX_SCAN,
		FIND_BEST_NEIGHBOR,
		REALLOCATE_CLUSTERS,
		REDUCE_PREFIX_SCAN,
		RESET_LAST_POSITION_PREFIX_SCAN,

		// Radix sort
		BIT_MASK_RADIX_SORT,
		END_LOOP_COMPUTATIONS,
		REALLOCATE_RADIX_SORT,
		RESET_BUFFER_INDEX,

		// Model
		COMPUTE_TANGENTS_1,
		COMPUTE_TANGENTS_2,
		MODEL_APPLY_MODEL_MATRIX,
		MODEL_MESH_GENERATION,
		PLANAR_SURFACE_GENERATION,
		PLANAR_SURFACE_TOPOLOGY,

		// Point cloud
		ADD_COLORS_HQR,
		COMPUTE_MORTON_CODES_PCL,
		IOTA_SHADER,
		REDUCE_POINT_BUFFER_SHADER,
		RESET_DEPTH_BUFFER_SHADER,
		RESET_DEPTH_BUFFER_HQR_SHADER,
		PROJECTION_SHADER,
		PROJECTION_HQR_SHADER,
		STORE_TEXTURE_SHADER,
		STORE_TEXTURE_HQR_SHADER,
		TRANSFER_POINTS_SHADER
	};

	/**
	*	@return Number of compute shaders.
	*/
	const static GLsizei numComputeShaderTypes() { return TRANSFER_POINTS_SHADER + 1; }

	/**
	*	@return Number of rendering shaders.
	*/
	const static GLsizei numRenderingShaderTypes() { return DEBUG_QUAD_SHADER + 1; }

	/// [Rendering parameters]

	// Matrices types
	enum MatricesTypes : uint8_t
	{
		MODEL_MATRIX,
		VIEW_MATRIX,
		PROJ_MATRIX,
		VIEW_PROJ_MATRIX,
		BIAS_VIEW_PROJ_MATRIX
	};

	/**
	*	@return Number of rendering shaders.
	*/
	const static GLsizei numMatricesTypes() { return BIAS_VIEW_PROJ_MATRIX + 1; }
};