#pragma once

#include "Graphics/Core/Model3D.h"
#include "tinyply/tinyply.h"

/**
*	@file Group3D.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/09/2019
*/

#define BVH_NODE_INDICES 36					// 12 LINES x 2 VERTICES + 12 (RESTART_PRIMITIVE_INDEX)

/**
*	@brief Wrapper for several 3d models which inherites from Model3D.
*/
class Group3D: public Model3D
{
public:
	struct VolatileGPUData;
	struct VolatileGroupData;
	struct StaticGPUData;

protected:
	const static GLuint					BVH_BUILDING_RADIUS;			//!< Radius to search nearest neighbors in BVH building process
		
protected:
	AABB								_aabb;							//!< Boundaries of all those objects defined behind this group
	std::vector<ModelComponent*>		_globalModelComp;				//!< Wraps every model component from the group
	unsigned							_numClusters;					//!< Total number of BVH nodes
	std::vector<Model3D*>				_objects;						//!< Elements which take part of the group

	// [GPU data]
	StaticGPUData*						_staticGPUData;					//!<

	// [Rendering status]
	VAO*								_bvhVAO;						//!< VAO which allows us to render current tree level

protected:
	/**
	*	@brief Builds the buffers which are necessary to build the BVH.
	*/
	void aggregateSSBOData(VolatileGPUData*& volatileGPUData, StaticGPUData*& staticGPUData);
	
	/**
	*	@brief Builds the VAO which allows us to render the BVH levels.
	*/
	void buildBVHVAO(VolatileGPUData* gpuData);
	
	/**
	*	@brief Builds the input buffer which is neeeded as input array to generate BVH.
	*	@param sortedFaces GPU buffer where face indices are sortered according to their Morton codes.
	*/
	void buildClusterBuffer(VolatileGPUData* gpuData, const GLuint sortedFaces);

	/**
	*	@brief Computes the AABB which wraps all the triangles contained in this group.
	*	@param triangleBufferID To compute the AABB we need to define the triangle buffer in GPU, so that is saved to be reused.
	*/
	virtual AABB computeAABB(VolatileGroupData* groupData);

	/**
	*	@brief Computes the morton codes for each triangle boundind box.
	*/
	GLuint computeMortonCodes();

	/**
	*	@brief Rearranges the face array to sort it by morton codes.
	*	@param mortonCodes Computed morton codes from faces buffer.
	*/
	GLuint sortFacesByMortonCode(const GLuint mortonCodes);

	/**
	*	@brief Saves group objects in a PLY file. 
	*/
	void writeModelComponentsPly();

public:
	/**
	*	@brief Constructor.
	*	@param modelMatrix Transformation to be applied for the whole group.
	*/
	Group3D(const mat4& modelMatrix = mat4(1.0f));

	/**
	*	@brief Destructor.
	*/
	virtual ~Group3D();

	/**
	*	@brief Adds a new component in the group array.
	*/
	void addComponent(Model3D* object);

	/**
	*	@brief Builds the BVH from scratch once the group is loaded.
	*/
	StaticGPUData* generateBVH(bool buildVisualization = false);

	/**
	*	@brief Loads all those components who belong to this group, applying the model matrix linked to such group.
	*/
	virtual bool load(const mat4& modelMatrix = mat4(1.0f));

	/**
	*	@brief Assigns an id for a model component and registers it into an array.
	*/
	void registerModelComponent(ModelComponent* modelComp);

	/**
	*	@brief Extension from registerScene for inner nodes which are also groups.
	*/
	virtual void registerModelComponentGroup(Group3D* group);

	/**
	*	@brief Iterates through the group content to assign an id for each model component.
	*/
	void registerScene();

	// ------------------------- Getters -------------------------------

	/**
	*	@return Bounding volume of complete scene.
	*/
	AABB getAABB() { return _aabb; }

	/**
	*	@return Model component which is identifier by id param.
	*/
	ModelComponent* getModelComponent(unsigned id);

	/**
	*	@return Pointer of vector with all registered model components.
	*/
	std::vector<ModelComponent*>* getRegisteredModelComponents() { return &_globalModelComp; }

	// ---------------------------- Rendering ---------------------------------
	
	/**
	*	@brief Renders the model as a set of lines.
	*	@param shader Shader which will draw the model.
	*	@param shaderType Index of shader at the list, so we can identify what type of uniform variables must be declared.
	*	@param matrix Vector of matrices which can be applied in the rendering process.
	*/
	virtual void drawAsLines(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix);

	/**
	*	@brief Renders the model as a set of points.
	*	@param shader Shader which will draw the model.
	*	@param shaderType Index of shader at the list, so we can identify what type of uniform variables must be declared.
	*	@param matrix Vector of matrices which can be applied in the rendering process.
	*/
	virtual void drawAsPoints(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix);

	/**
	*	@brief Renders the model as a set of triangles.
	*	@param shader Shader which will draw the model.
	*	@param shaderType Index of shader at the list, so we can identify what type of uniform variables must be declared.
	*	@param matrix Vector of matrices which can be applied in the rendering process.
	*/
	virtual void drawAsTriangles(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix);

	/**
	*	@brief Renders the model as a set of triangles with reflection.
	*	@param shader Shader which will draw the model.
	*	@param shaderType Index of shader at the list, so we can identify what type of uniform variables must be declared.
	*	@param matrix Vector of matrices which can be applied in the rendering process.
	*/
	virtual void drawAsReflectiveTriangles(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix, Camera* camera, const GLuint lightIndex = 0);

	/**
	*	@brief Renders the model as a set of triangles with no texture as we only want to retrieve the depth.
	*	@param shader Shader which will draw the model.
	*	@param shaderType Index of shader at the list, so we can identify what type of uniform variables must be declared.
	*	@param matrix Vector of matrices which can be applied in the rendering process.
	*/
	virtual void drawAsTriangles4Shadows(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix);

	/**
	*	@brief Renders the current selected BVH level as a set of cubes.
	*	@param shader Shader which will draw the model.
	*	@param shaderType Index of shader at the list, so we can identify what type of uniform variables must be declared.
	*	@param matrix Vector of matrices which can be applied in the rendering process.
	*/
	virtual void drawBVH(RenderingShader* shader, const RendEnum::RendShaderTypes shaderType, std::vector<mat4>& matrix);

public:
	/**
	*	@brief
	*/
	struct VolatileGPUData
	{
		// [BVH]
		std::vector<BVHCluster>			_cluster;						//!< BVH nodes

		// [GPU buffers]
		GLuint							_tempClusterSSBO;				//!< Temporary buffer for BVH construction
		GLuint							_mortonCodesSSBO;				//!< Morton codes for each triangle

		/**
		*	@brief Default constructor.
		*/
		VolatileGPUData();

		/**
		*	@brief Destructor.
		*/
		~VolatileGPUData();
	};

	/**
	*	@brief
	*/
	struct VolatileGroupData
	{
		// [CPU generated data]
		std::vector<VertexGPUData>		_geometry;						//!< Global geometry
		std::vector<FaceGPUData>		_triangleMesh;					//!< Global face data
		std::vector<MeshGPUData>		_meshData;						//!< Global meshes
	};

	/**
	*	@brief 
	*/
	struct StaticGPUData
	{
		// [SSBO]
		GLuint							_groupGeometrySSBO;				//!< SSBO of group geometry
		GLuint							_groupTopologySSBO;				//!< SSBO of group faces
		GLuint							_groupMeshSSBO;					//!< SSBO of group meshes
		GLuint							_clusterSSBO;					//!< BVH nodes

		// [Metadata]
		unsigned						_numTriangles;					//!< Replaces cluster array data to release memory

		/**
		*	@brief Default constructor.
		*/
		StaticGPUData();

		/**
		*	@brief Destructor.
		*/
		~StaticGPUData();
	};
};

