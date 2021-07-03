#pragma once

#include "Graphics/Application/SSAOScene.h"
#include "Graphics/Core/PointCloud.h"
#include "Graphics/Core/PointCloudAggregator.h"

#define CAMERA_POS_HEADER "Position"
#define CAMERA_LOOKAT_HEADER "LookAt"

/**
*	@file CADScene.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 28/11/2020
*/

/**
*	@brief Renderer of the point cloud.
*/
class PointCloudScene: public SSAOScene
{
protected:
	// Settings constraints
	const static std::string SCENE_CAMERA_FILE;				//!<
	const static std::string POINT_CLOUD_FILE;				//!<

protected:
	PointCloud*				_pointCloud;
	PointCloudAggregator*	_pointCloudAggregator;

	// Rendering
	RenderingShader*		_quadRenderer;
	VAO*					_quadVAO;

protected:
	/**
	*	@brief Loads a camera with code-defined values.
	*/
	void loadDefaultCamera(Camera* camera);

	/**
	*	@brief Loads the cameras from where the scene is observed. If no file is accessible, then a default camera is loaded.
	*/
	virtual void loadCameras();

	/**
	*	@brief Loads the lights which affects the scene. If no file is accessible, then default lights are loaded.
	*/
	virtual void loadLights();

	/**
	*	@brief Loads the models which are necessary to render the scene.
	*/
	virtual void loadModels();

protected:
	// Rendering related
	/**
	*	@brief Binds the projected point cloud as a texture in the final shader. 
	*/
	void bindTexture(GLuint textureID, ShaderProgram* shader, const std::string& uniformName, unsigned offset);
	
	/**
	*	@brief Renders the scene as a set of points.
	*	@param mModel Additional model matrix to be applied over the initial model matrix.
	*	@param rendParams Rendering parameters to be taken into account.
	*/
	virtual void drawAsPoints(const mat4& mModel, RenderingParameters* rendParams);

public:
	/**
	*	@brief Default constructor.
	*/
	PointCloudScene();

	/**
	*	@brief Destructor. Frees memory allocated for 3d models.
	*/
	virtual ~PointCloudScene();

	/**
	*	@return True if the point cloud was successfully loaded.
	*/
	bool loadPointCloud(const std::string& path);

	/**
	*	@brief Resize event.
	*	@param width New canvas width.
	*	@param height New canvas height.
	*/
	virtual void modifySize(const uint16_t width, const uint16_t height);

	/**
	*	@brief Draws the scene as the rendering parameters specifies.
	*	@param mModel Additional model matrix to be applied over the initial model matrix.
	*	@param rendParams Rendering parameters to be taken into account.
	*/
	virtual void render(const mat4& mModel, RenderingParameters* rendParams);
};

