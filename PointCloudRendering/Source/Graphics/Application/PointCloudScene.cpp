#include "stdafx.h"
#include "PointCloudScene.h"

#include <filesystem>
#include <regex>
#include "Graphics/Application/Renderer.h"
#include "Graphics/Application/TextureList.h"
#include "Graphics/Core/CADModel.h"
#include "Graphics/Core/Light.h"
#include "Graphics/Core/OpenGLUtilities.h"

/// Initialization of static attributes
const std::string PointCloudScene::SCENE_CAMERA_FILE = "Camera.txt";
const std::string PointCloudScene::POINT_CLOUD_FILE = "Assets/Models/PointCloudSubsample01";

// [Public methods]

PointCloudScene::PointCloudScene() : _pointCloud(nullptr), _pointCloudAggregator(nullptr)
{
	ShaderList* shaderList = ShaderList::getInstance();

	_pointCloudAggregator = new PointCloudAggregator();
	_quadRenderer = shaderList->getRenderingShader(RendEnum::DEBUG_QUAD_SHADER);
	_quadVAO = Primitives::getQuadVAO();
}

PointCloudScene::~PointCloudScene()
{
	delete _pointCloud;
	delete _pointCloudAggregator;
}

bool PointCloudScene::loadPointCloud(const std::string& path)
{
	bool nullPointCloud = _pointCloud == nullptr;
	
	delete _pointCloud;
	_pointCloud = new PointCloud(path, true);
	if (!_pointCloud->load()) return false;
	_pointCloudAggregator->setPointCloud(_pointCloud);

	if (Renderer::getInstance()->getRenderingParameters()->_updateCamera) this->loadDefaultCamera(_cameraManager->getActiveCamera());

	std::cout << _pointCloud->getNumberOfPoints() << std::endl;

	return true;
}

void PointCloudScene::modifySize(const uint16_t width, const uint16_t height)
{
	Scene::modifySize(width, height);

	_pointCloudAggregator->changedSize(width, height);
}

void PointCloudScene::render(const mat4& mModel, RenderingParameters* rendParams)
{
	this->bindDefaultFramebuffer(rendParams);	
	this->renderPointCloud(mModel, rendParams);
}

// [Protected methods]

void PointCloudScene::drawAsPoints(const mat4& mModel, RenderingParameters* rendParams)
{
	Camera* activeCamera		= _cameraManager->getActiveCamera();
	const mat4 projectionMatrix = activeCamera->getViewProjMatrix() * mModel;

	_pointCloudAggregator->render(projectionMatrix);

	_quadRenderer->use();
	_quadRenderer->applyActiveSubroutines();
	this->bindTexture(_pointCloudAggregator->getTexture(), _quadRenderer, "texSampler", 0);

	_quadVAO->drawObject(RendEnum::IBO_TRIANGLE_MESH, GL_TRIANGLES, 2 * 4);
}

void PointCloudScene::loadDefaultCamera(Camera* camera)
{
	if (_pointCloud)
	{
		camera->setPosition(_pointCloud->getAABB().center() + _pointCloud->getAABB().extent() * 1.5f);
		camera->setLookAt(_pointCloud->getAABB().center());
	}
	else
	{
		camera->setPosition(vec3(3.0f, 3.338f, 2.94f));
		camera->setLookAt(vec3(-5.31f, 1.65f, -2.57f));
	}
}

void PointCloudScene::loadCameras()
{
	ivec2 canvasSize = _window->getSize();
	Camera* camera = new Camera(canvasSize[0], canvasSize[1]);

	this->loadDefaultCamera(camera);

	_cameraManager->insertCamera(camera);
}

void PointCloudScene::loadLights()
{
	Scene::loadLights();
}

void PointCloudScene::loadModels()
{
	{
		_sceneGroup = new Group3D;
		_pointCloudAggregator = new PointCloudAggregator();
	}
	
	SSAOScene::loadModels();
}

void PointCloudScene::bindTexture(GLuint textureID, ShaderProgram* shader, const std::string& uniformName, unsigned offset)
{
	shader->setUniform(uniformName, offset);
	glActiveTexture(GL_TEXTURE0 + offset);
	glBindTexture(GL_TEXTURE_2D, textureID);
}
