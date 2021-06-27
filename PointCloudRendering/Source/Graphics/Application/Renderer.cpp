#include "stdafx.h"
#include "Renderer.h"

#include "Graphics/Application/PointCloudScene.h"
#include "Graphics/Core/PointCloudAggregator.h"
#include "Interface/Window.h"
#include "Utilities/FileManagement.h"

// [Static attributes]

const std::string Renderer::SCENE_INDEX_FILENAME = "Settings/SceneIndex.txt";

/// [Protected methods]

Renderer::Renderer() :
	_screenshotFBO(nullptr),
	_scene(CGAppEnum::numAvailableScenes()),
	_state(std::unique_ptr<RenderingParameters>(new RenderingParameters()))
{
	_currentScene = this->readSceneIndex();
}

SSAOScene* Renderer::createScene(const uint8_t sceneType)
{
	return new PointCloudScene();
}

uint8_t Renderer::readSceneIndex()
{
	int index;
	uint8_t scene = CGAppEnum::TERRAIN_SCENE;		// Default

	// Read file
	std::ifstream fin(SCENE_INDEX_FILENAME, std::ios::in);
	if (fin.is_open())
	{
		fin >> index;
		
		scene = glm::clamp(index, (int) CGAppEnum::TERRAIN_SCENE, (int) CGAppEnum::CAD_SCENE);
	}

	fin.close();

	return scene;
}

/// [Public methods]

Renderer::~Renderer()
{
}

void Renderer::prepareOpenGL(const uint16_t width, const uint16_t height)
{
	glClearColor(_state->_backgroundColor.x, _state->_backgroundColor.y, _state->_backgroundColor.z, 1.0f);

	glEnable(GL_DEPTH_TEST);							// Depth is taking into account when drawing and only points with LESS depth are taken into account
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND);									// Multipass rendering

	glEnable(GL_MULTISAMPLE);							// Antialiasing

	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);					// Point clouds of different point sizes

	glEnable(GL_POLYGON_OFFSET_FILL);					// Several topologies at the same time
	glPolygonOffset(0.5f, 0.5f);

	glCullFace(GL_FRONT);								// Necessary for shadow mapping, even tho we need an enable order before it

	glEnable(GL_PRIMITIVE_RESTART);						// Index which marks different primitives
	glPrimitiveRestartIndex(Model3D::RESTART_PRIMITIVE_INDEX);

	ComputeShader::initializeMaxGroupSize();			// Once the context is ready we can query for maximum work group size
	Model3D::buildShadowOffsetTexture();				// Alternative shadow technique
	Model3D::buildSSAONoiseKernels();					// Ambient occlusion samples

	// [State]

	_state->_viewportSize = ivec2(width, height);

	// [Scenes]

	_scene[_currentScene] = std::unique_ptr<SSAOScene>(createScene(_currentScene));
	_scene[_currentScene]->load();

	// [Framebuffers]

	_screenshotFBO = std::unique_ptr<FBOScreenshot>(new FBOScreenshot(width, height));
}

void Renderer::render()
{
	_scene[_currentScene]->render(glm::rotate(mat4(1.0f), -glm::pi<float>() / 2.0f, vec3(1.0f, .0f, .0f)), _state.get());
}

bool Renderer::getScreenshot(const std::string& filename)
{
	const ivec2 size = _state->_viewportSize;
	const ivec2 newSize = ivec2(_state->_viewportSize.x * _state->_screenshotMultiplier, _state->_viewportSize.y * _state->_screenshotMultiplier);

	_scene[_currentScene]->modifyNextFramebufferID(_screenshotFBO->getIdentifier());
	this->resize(newSize.x, newSize.y);
	Window::getInstance()->changedSize(newSize.x, newSize.y);

	this->render();
	const bool success = _screenshotFBO->saveImage(filename);

	_scene[_currentScene]->modifyNextFramebufferID(0);
	this->resize(size.x, size.y);
	Window::getInstance()->changedSize(size.x, size.y);

	return success;
}

void Renderer::resize(const uint16_t width, const uint16_t height)
{
	// Viewport state
	_state->_viewportSize = ivec2(width, height);

	// OpenGL
	glViewport(0, 0, width, height);

	// Scenes
	_scene[_currentScene]->modifySize(width, height);

	// FBO
	_screenshotFBO->modifySize(width, height);
}
