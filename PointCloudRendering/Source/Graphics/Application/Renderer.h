#pragma once

#include "Graphics/Application/CameraManager.h"
#include "Graphics/Application/GraphicsAppEnumerations.h"
#include "Graphics/Application/SSAOScene.h"
#include "Graphics/Application/RenderingParameters.h"
#include "Graphics/Application/Scene.h"
#include "Graphics/Core/Camera.h"
#include "Graphics/Core/FBOScreenshot.h"
#include "Graphics/Core/Model3D.h"
#include "Graphics/Core/RenderingShader.h"
#include "Utilities/Singleton.h"

/**
*	@file InputManager.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/11/2019
*/

/**
*	@brief Manages the scenes and graphic application state.
*/
class Renderer: public Singleton<Renderer>
{
	friend class Singleton<Renderer>;

protected:
	const static std::string SCENE_INDEX_FILENAME;

protected:
	// [Rendering]
	uint8_t										_currentScene;			//!< Active scene
	std::unique_ptr<FBOScreenshot>				_screenshotFBO;			//!< Framebuffer which allows us to capture the scene (and save it) at higher resolution
	std::vector<std::unique_ptr<SSAOScene>>	_scene;					//!< Array of scenes which can be represented. Components are only initialized when asked for
	std::unique_ptr<RenderingParameters>		_state;					//!< Properties of rendering

protected:
	/**
	*	@brief Default constructor with the only purpose of initializing properly the attributes.
	*/
	Renderer();

	/**
	*	@brief Creates an scene taking into account the type from the arguments.
	*	@param sceneType Index-name of scene type.
	*/
	SSAOScene* createScene(const uint8_t sceneType);

	/**
	*	@brief Reads the index of the scene which needs to be loaded at first.
	*/
	uint8_t readSceneIndex();

public:
	/**
	*	@brief Destructor. Frees resources.
	*/
	virtual ~Renderer();

	/**
	*	@brief Initializes the OpenGL variables at the current context.
	*	@param width Canvas initial width.
	*	@param height Canvas initial height.
	*/
	void prepareOpenGL(const uint16_t width, const uint16_t height);

	/**
	*	@brief Draws the currently active scene.
	*/
	void render();

	// [Scene data]

	/**
	*	@return Current camera that allows us to capture the scene.
	*/
	Camera* getActiveCamera()  const { return _scene[_currentScene]->getCameraManager()->getActiveCamera(); }

	/**
	*	@return Color of background under the scene.
	*/
	vec3 getBackgroundColor()  const { return _state->_backgroundColor; }

	/**
	*	@return Currently active scene.
	*/
	SSAOScene* getCurrentScene() { return _scene[_currentScene].get(); }

	/**
	*	@return Wrapping class for rendering parameters (unique instance per renderer).
	*/
	RenderingParameters* getRenderingParameters() { return _state.get(); }

	/**
	*	@brief Renders the scene into an FBO.
	*	@param filename Path of file system where the image needs to be saved.
	*/
	bool getScreenshot(const std::string& filename);

	// [Events]

	/**
	*	@brief Resize event from canvas view.
	*	@param width New canvas width.
	*	@param height New canvas height.
	*/
	void resize(const uint16_t width, const uint16_t height);
};

