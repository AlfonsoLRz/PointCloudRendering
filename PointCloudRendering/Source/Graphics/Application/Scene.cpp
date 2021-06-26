#include "stdafx.h"
#include "Scene.h"

#include "Graphics/Core/OpenGLUtilities.h"
#include "Graphics/Core/ShaderList.h"

/// [Public methods]

Scene::Scene():
	_cameraManager(std::unique_ptr<CameraManager>(new CameraManager())), _sceneGroup(nullptr),
	_nextFramebufferID(0), _ssaoFBO(new SSAOFBO())
{
	_window = Window::getInstance();
}

Scene::~Scene()
{
	delete _sceneGroup;
	delete _ssaoFBO;
}

void Scene::load()
{
	this->loadCameras();
	this->loadLights();
	this->loadModels();
}

void Scene::modifyNextFramebufferID(const GLuint fboID)
{
	_nextFramebufferID = fboID;
}

void Scene::modifySize(const uint16_t width, const uint16_t height)
{
	_cameraManager->getActiveCamera()->setRaspect(width, height);
	_ssaoFBO->modifySize(width, height);
}

void Scene::render(const mat4& mModel, RenderingParameters* rendParams)
{
	Camera* activeCamera = _cameraManager->getActiveCamera();

	this->drawAsTriangles4Shadows(mModel, rendParams);

	if (rendParams->_ambientOcclusion)
	{
		_ssaoFBO->bindMultisamplingFBO();
		this->drawAsTriangles(mModel, rendParams);
		_ssaoFBO->writeGBuffer(0);

		_ssaoFBO->bindGBufferFBO(1);
		if (rendParams->_showTriangleMesh) this->drawAsTriangles4Position(mModel, rendParams);

		_ssaoFBO->bindGBufferFBO(2);
		if (rendParams->_showTriangleMesh) this->drawAsTriangles4Normal(mModel, rendParams);

		_ssaoFBO->bindSSAOFBO();
		this->drawSSAOScene();

		this->bindDefaultFramebuffer(rendParams);
		this->composeScene();
	}
	else
	{
		this->bindDefaultFramebuffer(rendParams);

		this->drawAsTriangles(mModel, rendParams);
	}
}

// [Protected methods]

void Scene::drawAsPoints(const mat4& mModel, RenderingParameters* rendParams)
{
	Camera* activeCamera = _cameraManager->getActiveCamera(); if (!activeCamera) return;
	RenderingShader* shader = ShaderList::getInstance()->getRenderingShader(RendEnum::POINT_CLOUD_SHADER);
	std::vector<mat4> matrix(RendEnum::numMatricesTypes());

	matrix[RendEnum::MODEL_MATRIX] = mModel;
	matrix[RendEnum::VIEW_PROJ_MATRIX] = activeCamera->getViewProjMatrix();

	shader->use();
	shader->applyActiveSubroutines();

	this->drawSceneAsPoints(shader, RendEnum::POINT_CLOUD_SHADER, &matrix, rendParams);
}

void Scene::drawAsLines(const mat4& mModel, RenderingParameters* rendParams)
{
	Camera* activeCamera = _cameraManager->getActiveCamera(); if (!activeCamera) return;
	RenderingShader* shader = ShaderList::getInstance()->getRenderingShader(RendEnum::WIREFRAME_SHADER);
	std::vector<mat4> matrix(RendEnum::numMatricesTypes());

	matrix[RendEnum::MODEL_MATRIX] = mModel;
	matrix[RendEnum::VIEW_PROJ_MATRIX] = activeCamera->getViewProjMatrix();

	shader->use();
	shader->applyActiveSubroutines();

	this->drawSceneAsLines(shader, RendEnum::WIREFRAME_SHADER, &matrix, rendParams);
}

void Scene::drawAsTriangles(const mat4& mModel, RenderingParameters* rendParams)
{
	Camera* activeCamera = _cameraManager->getActiveCamera(); if (!activeCamera) return;
	this->drawAsTriangles(activeCamera, mModel, rendParams);
}

void Scene::drawAsTriangles(Camera* camera, const mat4& mModel, RenderingParameters* rendParams)
{
	RenderingShader* shader = ShaderList::getInstance()->getRenderingShader(RendEnum::TRIANGLE_MESH_SHADER);
	std::vector<mat4> matrix(RendEnum::numMatricesTypes());
	const mat4 bias = glm::translate(mat4(1.0f), vec3(0.5f)) * glm::scale(mat4(1.0f), vec3(0.5f));						// Proj: [-1, 1] => with bias: [0, 1]

	{
		matrix[RendEnum::MODEL_MATRIX] = mModel;
		matrix[RendEnum::VIEW_MATRIX] = camera->getViewMatrix();
		matrix[RendEnum::VIEW_PROJ_MATRIX] = camera->getViewProjMatrix();

		glDepthFunc(GL_LEQUAL);

		shader->use();
		shader->setUniform("materialScattering", rendParams->_materialScattering);										// Ambient lighting
	}

	{
		for (unsigned int i = 0; i < _lights.size(); ++i)																// Multipass rendering
		{
			if (i == 0)
			{
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			else
			{
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			}

			if (_lights[i]->shouldCastShadows())
			{
				matrix[RendEnum::BIAS_VIEW_PROJ_MATRIX] = bias * _lights[i]->getCamera()->getViewProjMatrix();
			}

			_lights[i]->applyLight(shader, matrix[RendEnum::VIEW_MATRIX]);
			_lights[i]->applyShadowMapTexture(shader);
			shader->applyActiveSubroutines();

			this->drawSceneAsTriangles(shader, RendEnum::TRIANGLE_MESH_SHADER, &matrix, rendParams);
		}
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);					// Back to initial state
	glDepthFunc(GL_LESS);
}

void Scene::drawAsTriangles4Position(const mat4& mModel, RenderingParameters* rendParams)
{
	Camera* activeCamera = _cameraManager->getActiveCamera(); if (!activeCamera) return;
	RenderingShader* shader = ShaderList::getInstance()->getRenderingShader(RendEnum::TRIANGLE_MESH_POSITION_SHADER);
	std::vector<mat4> matrix(RendEnum::numMatricesTypes());

	{
		matrix[RendEnum::MODEL_MATRIX] = mModel;
		matrix[RendEnum::VIEW_MATRIX] = activeCamera->getViewMatrix();
		matrix[RendEnum::VIEW_PROJ_MATRIX] = activeCamera->getViewProjMatrix();

		shader->use();

		this->drawSceneAsTriangles4Position(shader, RendEnum::TRIANGLE_MESH_POSITION_SHADER, &matrix, rendParams);
	}
}

void Scene::drawAsTriangles4Normal(const mat4& mModel, RenderingParameters* rendParams)
{
	Camera* activeCamera = _cameraManager->getActiveCamera(); if (!activeCamera) return;
	RenderingShader* shader = ShaderList::getInstance()->getRenderingShader(RendEnum::TRIANGLE_MESH_NORMAL_SHADER);
	std::vector<mat4> matrix(RendEnum::numMatricesTypes());

	{
		matrix[RendEnum::MODEL_MATRIX] = mModel;
		matrix[RendEnum::VIEW_MATRIX] = activeCamera->getViewMatrix();
		matrix[RendEnum::VIEW_PROJ_MATRIX] = activeCamera->getViewProjMatrix();

		shader->use();

		this->drawSceneAsTriangles4Normal(shader, RendEnum::TRIANGLE_MESH_POSITION_SHADER, &matrix, rendParams);
	}
}

void Scene::drawAsTriangles4Shadows(const mat4& mModel, RenderingParameters* rendParams)
{
	Camera* activeCamera = _cameraManager->getActiveCamera(); if (!activeCamera) return;
	RenderingShader* shader = ShaderList::getInstance()->getRenderingShader(RendEnum::SHADOWS_SHADER);
	const ivec2 canvasSize = rendParams->_viewportSize;
	std::vector<mat4> matrix(RendEnum::numMatricesTypes());

	{
		matrix[RendEnum::MODEL_MATRIX] = mModel;

		glEnable(GL_CULL_FACE);

		shader->use();
	}

	for (unsigned int i = 0; i < _lights.size(); ++i)
	{
		Light* light = _lights[i].get();

		if (light->shouldCastShadows() && _computeShadowMap[i])
		{
			ShadowMap* shadowMap = light->getShadowMap();
			const ivec2 size = shadowMap->getSize();

			{
				shadowMap->bindFBO();
				glClear(GL_DEPTH_BUFFER_BIT);
				glViewport(0, 0, size.x, size.y);
				shader->applyActiveSubroutines();
			}

			matrix[RendEnum::VIEW_PROJ_MATRIX] = light->getCamera()->getViewProjMatrix();

			_sceneGroup->drawAsTriangles4Shadows(shader, RendEnum::SHADOWS_SHADER, matrix);

			_computeShadowMap[i] = false;
		}
	}

	glViewport(0, 0, canvasSize.x, canvasSize.y);
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::drawSceneAsPoints(RenderingShader* shader, RendEnum::RendShaderTypes shaderType, std::vector<mat4>* matrix, RenderingParameters* rendParams)
{
	_sceneGroup->drawAsPoints(shader, shaderType, *matrix);
}

void Scene::drawSceneAsLines(RenderingShader* shader, RendEnum::RendShaderTypes shaderType, std::vector<mat4>* matrix, RenderingParameters* rendParams)
{
	_sceneGroup->drawAsLines(shader, shaderType, *matrix);
}

void Scene::drawSceneAsTriangles(RenderingShader* shader, RendEnum::RendShaderTypes shaderType, std::vector<mat4>* matrix, RenderingParameters* rendParams)
{
	_sceneGroup->drawAsTriangles(shader, shaderType, *matrix);
}

void Scene::drawSceneAsTriangles4Normal(RenderingShader* shader, RendEnum::RendShaderTypes shaderType, std::vector<mat4>* matrix, RenderingParameters* rendParams)
{
	_sceneGroup->drawAsTriangles4Shadows(shader, shaderType, *matrix);
}

void Scene::drawSceneAsTriangles4Position(RenderingShader* shader, RendEnum::RendShaderTypes shaderType, std::vector<mat4>* matrix, RenderingParameters* rendParams)
{
	_sceneGroup->drawAsTriangles4Shadows(shader, shaderType, *matrix);
}

void Scene::bindDefaultFramebuffer(RenderingParameters* rendParams)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _nextFramebufferID);
	glClearColor(rendParams->_backgroundColor.x, rendParams->_backgroundColor.y, rendParams->_backgroundColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene::composeScene()
{
	RenderingShader* shader = ShaderList::getInstance()->getRenderingShader(RendEnum::BLUR_SSAO_SHADER);
	shader->use();

	_ssaoFBO->bindSSAOTexture(shader);

	VAO* quadVAO = Primitives::getQuadVAO();
	quadVAO->drawObject(RendEnum::IBO_TRIANGLE_MESH, GL_TRIANGLES, 2 * 4);			// 2 triangles with 3 indices + 1 boundary index
}

void Scene::drawSSAOScene()
{
	Camera* activeCamera = _cameraManager->getActiveCamera(); if (!activeCamera) return;
	RenderingShader* shader = ShaderList::getInstance()->getRenderingShader(RendEnum::SSAO_SHADER);
	shader->use();

	_ssaoFBO->bindGBufferTextures(shader);
	Model3D::bindSSAOTextures(shader);
	shader->setUniform("windowSize", vec2(_window->getSize()));
	shader->setUniform("mProjMatrix", activeCamera->getProjectionMatrix());

	VAO* quadVAO = Primitives::getQuadVAO();
	quadVAO->drawObject(RendEnum::IBO_TRIANGLE_MESH, GL_TRIANGLES, 2 * 4);		
}

void Scene::loadCameras()
{
	ivec2 canvasSize = Window::getInstance()->getSize();
	Camera* camera = new Camera(canvasSize[0], canvasSize[1]);

	_cameraManager->insertCamera(camera);
}

void Scene::loadLights()
{
	_computeShadowMap = std::vector<bool>(_lights.size());
	for (unsigned int i = 0; i < _computeShadowMap.size(); ++i)
	{
		_computeShadowMap[i] = true;
	}
}
