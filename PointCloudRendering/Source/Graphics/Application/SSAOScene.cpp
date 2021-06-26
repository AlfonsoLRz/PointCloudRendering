#include "stdafx.h"
#include "SSAOScene.h"

#include "Geometry/3D/Intersections3D.h"
#include "Graphics/Application/Renderer.h"
#include "Graphics/Application/TextureList.h"
#include "Graphics/Core/OpenGLUtilities.h"
#include "Graphics/Core/RenderingShader.h"
#include "Graphics/Core/ShaderList.h"
#include "Interface/Window.h"

/// [Public methods]

SSAOScene::SSAOScene(): Scene(),
	_releaseMemory(true)
{
}

SSAOScene::~SSAOScene()
{
}

void SSAOScene::load()
{	
	Scene::load();
}

void SSAOScene::render(const mat4& mModel, RenderingParameters* rendParams)
{
	Camera* activeCamera = _cameraManager->getActiveCamera();

	this->drawAsTriangles4Shadows(mModel, rendParams);

	if (rendParams->_ambientOcclusion && this->needToApplyAmbientOcclusion(rendParams))
	{
		_ssaoFBO->bindMultisamplingFBO();
		this->renderScene(mModel, rendParams);
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

		this->renderScene(mModel, rendParams);
	}
}

void SSAOScene::render4Reflection(Camera* camera, const mat4& mModel, RenderingParameters* rendParams)
{
	this->drawAsTriangles(camera, mModel, rendParams);
}

/// [Protected methods]

void SSAOScene::loadModels()
{
}

bool SSAOScene::needToApplyAmbientOcclusion(RenderingParameters* rendParams)
{
	return !(rendParams->_visualizationMode == CGAppEnum::VIS_POINTS || rendParams->_visualizationMode == CGAppEnum::VIS_LINES);
}

void SSAOScene::renderOtherStructures(const mat4& mModel, RenderingParameters* rendParams)
{
	if (rendParams->_showBVH) this->renderBVH(mModel, rendParams);
}

void SSAOScene::renderScene(const mat4& mModel, RenderingParameters* rendParams)
{
	int visualizationMode = rendParams->_visualizationMode;

	switch (visualizationMode)
	{
	case CGAppEnum::VIS_POINTS:
		this->renderPointCloud(mModel, rendParams);
		break;
	case CGAppEnum::VIS_LINES:
		this->renderWireframe(mModel, rendParams);
		break;
	case CGAppEnum::VIS_TRIANGLES:
		this->renderTriangleMesh(mModel, rendParams);
		break;
	case CGAppEnum::VIS_ALL_TOGETHER:
		this->renderTriangleMesh(mModel, rendParams);
		this->renderWireframe(mModel, rendParams);
		this->renderPointCloud(mModel, rendParams);
		break;
	}

	this->renderOtherStructures(mModel, rendParams);
}

void SSAOScene::renderPointCloud(const mat4& mModel, RenderingParameters* rendParams)
{
	this->drawAsPoints(mModel, rendParams);
}

void SSAOScene::renderWireframe(const mat4& mModel, RenderingParameters* rendParams)
{
	this->drawAsLines(mModel, rendParams);
}

void SSAOScene::renderTriangleMesh(const mat4& mModel, RenderingParameters* rendParams)
{
	if (rendParams->_showTriangleMesh)
	{
		SSAOScene::drawAsTriangles(mModel, rendParams);
	}
}

// ---------------------------- Other structures -----------------------------

void SSAOScene::renderBVH(const mat4& model, RenderingParameters* rendParams)
{
	Camera* activeCamera = _cameraManager->getActiveCamera(); if (!activeCamera) return;
	std::vector<mat4> matrix(RendEnum::numMatricesTypes());

	matrix[RendEnum::MODEL_MATRIX] = model;
	matrix[RendEnum::VIEW_MATRIX] = activeCamera->getViewMatrix();
	matrix[RendEnum::VIEW_PROJ_MATRIX] = activeCamera->getViewProjMatrix();

	// BVH rendering
	{
		RenderingShader* shader = ShaderList::getInstance()->getRenderingShader(RendEnum::WIREFRAME_SHADER);
		shader->use();
		shader->applyActiveSubroutines();

		_sceneGroup->drawBVH(shader, RendEnum::WIREFRAME_SHADER, matrix);
	}
}

// ------------------------- Draw scene ------------------------------

void SSAOScene::drawSceneAsTriangles(RenderingShader* shader, RendEnum::RendShaderTypes shaderType, std::vector<mat4>* matrix, RenderingParameters* rendParams)
{
	Scene::drawSceneAsTriangles(shader, shaderType, matrix, rendParams);
}

void SSAOScene::drawSceneAsTriangles4Normal(RenderingShader* shader, RendEnum::RendShaderTypes shaderType, std::vector<mat4>* matrix, RenderingParameters* rendParams)
{
	Scene::drawSceneAsTriangles4Normal(shader, shaderType, matrix, rendParams);
}

void SSAOScene::drawSceneAsTriangles4Position(RenderingShader* shader, RendEnum::RendShaderTypes shaderType, std::vector<mat4>* matrix, RenderingParameters* rendParams)
{
	Scene::drawSceneAsTriangles4Position(shader, shaderType, matrix, rendParams);
}
