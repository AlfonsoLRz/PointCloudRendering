#include "stdafx.h"
#include "PointCloudScene.h"

#include <filesystem>
#include <regex>
#include "Graphics/Application/TextureList.h"
#include "Graphics/Core/CADModel.h"
#include "Graphics/Core/Light.h"
#include "Graphics/Core/OpenGLUtilities.h"

/// Initialization of static attributes
const std::string PointCloudScene::SCENE_CAMERA_FILE = "Camera.txt";
const std::string PointCloudScene::POINT_CLOUD_FILE = "Assets/Models/PointCloud";

// [Public methods]

PointCloudScene::PointCloudScene() : _pointCloud(nullptr), _pointCloudAggregator(nullptr)
{
	ShaderList* shaderList = ShaderList::getInstance();
	
	_quadRenderer = shaderList->getRenderingShader(RendEnum::DEBUG_QUAD_SHADER);
	_quadVAO = Primitives::getQuadVAO();
}

PointCloudScene::~PointCloudScene()
{
	delete _pointCloud;
	delete _pointCloudAggregator;
}

void PointCloudScene::modifySize(const uint16_t width, const uint16_t height)
{
	Scene::modifySize(width, height);

	
}

void PointCloudScene::render(const mat4& mModel, RenderingParameters* rendParams)
{
	this->bindDefaultFramebuffer(rendParams);
	
	this->renderPointCloud(mModel, rendParams);
}

// [Protected methods]

void PointCloudScene::drawAsPoints(const mat4& mModel, RenderingParameters* rendParams)
{
	Texture* texture			= TextureList::getInstance()->getTexture(CGAppEnum::TEXTURE_LIDAR_HEIGHT);
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
	camera->setPosition(vec3(3.0f, 3.338f, 2.94f));
	camera->setLookAt(vec3(-5.31f, 1.65f, -2.57f));
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
		
		_pointCloud = new PointCloud(POINT_CLOUD_FILE, true);
		_pointCloud->load();
		_pointCloudAggregator = new PointCloudAggregator(_pointCloud);
	}
	
	SSAOScene::loadModels();
}

void PointCloudScene::bindTexture(GLuint textureID, ShaderProgram* shader, const std::string& uniformName, unsigned offset)
{
	shader->setUniform(uniformName, offset);
	glActiveTexture(GL_TEXTURE0 + offset);
	glBindTexture(GL_TEXTURE_2D, textureID);
}
