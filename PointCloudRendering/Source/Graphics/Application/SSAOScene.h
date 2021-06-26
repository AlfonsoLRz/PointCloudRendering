#pragma once

#include "Graphics/Application/Scene.h"
#include "Graphics/Core/CADModel.h"
#include "Graphics/Core/DrawAABB.h"
#include "Graphics/Core/Model3D.h"
#include "Graphics/Core/ShaderList.h"

/**
*	@file LiDARScene.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/12/2019
*/

/**
*	@brief Generic scene for LiDAR simulation.
*/
class SSAOScene: public Scene
{
protected:
	// [Simulation]
	bool						_releaseMemory;				//!< Release memory after BVH initialization

protected:
	/**
	*	@brief Loads the models necessary to render the scene.
	*/
	virtual void loadModels();

	/**
	*	@return False if any point cloud or wireframe scene must be rendered.
	*/
	bool needToApplyAmbientOcclusion(RenderingParameters* rendParams);

	/**
	*	@brief Renders other data structures related to a LiDAR scene.
	*	@param mModel Additional model matrix to be applied over the initial model matrix.
	*	@param rendParams Rendering parameters to be taken into account.
	*/
	void renderOtherStructures(const mat4& mModel, RenderingParameters* rendParams);

	/**
	*	@brief Renders the scene without any post-processing efect.
	*/
	void renderScene(const mat4& mModel, RenderingParameters* rendParams);

	/**
	*	@brief Renders the scenario as a point cloud.
	*	@param mModel Additional model matrix to be applied over the initial model matrix.
	*	@param rendParams Rendering parameters to be taken into account.
	*/
	void renderPointCloud(const mat4& mModel, RenderingParameters* rendParams);

	/**
	*	@brief Renders the scenario as a wireframe mesh.
	*	@param mModel Additional model matrix to be applied over the initial model matrix.
	*	@param rendParams Rendering parameters to be taken into account.
	*/
	void renderWireframe(const mat4& mModel, RenderingParameters* rendParams);

	/**
	*	@brief Renders the triangle mesh scenario, whether it is rendered with real colors or semantic concepts.
	*	@param mModel Additional model matrix to be applied over the initial model matrix.
	*	@param rendParams Rendering parameters to be taken into account.
	*/
	void renderTriangleMesh(const mat4& mModel, RenderingParameters* rendParams);

	// --------------- Other secondary structures ----------------------
	
	/**
	*	@brief Renders BVH tree built by room group.
	*/
	void renderBVH(const mat4& model, RenderingParameters* rendParams);

	// -------------- Draw scene -------------

	/**
	*	@brief Decides which objects are going to be rendered as a triangle mesh.
	*	@param shader Rendering shader which is drawing the scene.
	*	@param shaderType Unique ID of "shader".
	*	@param matrix Vector of matrices, including view, projection, etc.
	*	@param rendParams Parameters which indicates how the scene is rendered.
	*/
	virtual void drawSceneAsTriangles(RenderingShader* shader, RendEnum::RendShaderTypes shaderType, std::vector<mat4>* matrix, RenderingParameters* rendParams);

	/**
	*	@brief Decides which objects are going to be rendered as a triangle mesh. Only the normal is calculated for each fragment.
	*	@param shader Rendering shader which is drawing the scene.
	*	@param shaderType Unique ID of "shader".
	*	@param matrix Vector of matrices, including view, projection, etc.
	*	@param rendParams Parameters which indicates how the scene is rendered.
	*/
	virtual void drawSceneAsTriangles4Normal(RenderingShader* shader, RendEnum::RendShaderTypes shaderType, std::vector<mat4>* matrix, RenderingParameters* rendParams);

	/**
	*	@brief Decides which objects are going to be rendered as a triangle mesh. Only the position is calculated for each fragment.
	*	@param shader Rendering shader which is drawing the scene.
	*	@param shaderType Unique ID of "shader".
	*	@param matrix Vector of matrices, including view, projection, etc.
	*	@param rendParams Parameters which indicates how the scene is rendered.
	*/
	virtual void drawSceneAsTriangles4Position(RenderingShader* shader, RendEnum::RendShaderTypes shaderType, std::vector<mat4>* matrix, RenderingParameters* rendParams);

public:
	/**
	*	@brief Default constructor.
	*/
	SSAOScene();

	/**
	*	@brief Destructor. Frees memory allocated for 3d models.
	*/
	virtual ~SSAOScene();

	/**
	*	@brief Loads the elements from the scene: lights, cameras, models, etc.
	*/
	virtual void load();

	/**
	*	@brief Draws the scene as the rendering parameters specifies.
	*	@param mModel Additional model matrix to be applied over the initial model matrix.
	*	@param rendParams Rendering parameters to be taken into account.
	*/
	virtual void render(const mat4& mModel, RenderingParameters* rendParams);

	/**
	*	@brief Draws the scene as the rendering parameters specifies.
	*	@param mModel Additional model matrix to be applied over the initial model matrix.
	*	@param rendParams Rendering parameters to be taken into account.
	*/
	virtual void render4Reflection(Camera* camera, const mat4& mModel, RenderingParameters* rendParams);

	// ------------ Getters ---------------

	/**
	*	@return Group which wraps the scene content.
	*/
	virtual Group3D* getRenderingGroup() { return _sceneGroup; }

	/**
	*	@return Axis aligned bounding box which wraps the complete scene.
	*/
	AABB getAABB() { return _sceneGroup->getAABB(); }

	/**
	*	@return Pointer of vector with all registered model components.
	*/
	std::vector<Model3D::ModelComponent*>* getModelComponents() { return _sceneGroup->getRegisteredModelComponents(); }
};

