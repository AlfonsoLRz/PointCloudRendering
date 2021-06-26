#include "stdafx.h"
#include "Light.h"

#include "Graphics/Application/Renderer.h"
#include "Graphics/Core/AmbientLight.h"
#include "Graphics/Core/BasicAttenuation.h"
#include "Graphics/Core/DirectionalLight.h"
#include "Graphics/Core/LightAttenuation.h"
#include "Graphics/Core/LightType.h"
#include "Graphics/Core/PixarAttenuation.h"
#include "Graphics/Core/PointLight.h"
#include "Graphics/Core/RangedAttenuation.h"
#include "Graphics/Core/RimLight.h"
#include "Graphics/Core/SpotLight.h"
#include "Interface/Window.h"

// [Static members initialization]

const std::unordered_map<std::string, Light::LightModels> Light::LIGHT_ENUM_STR = 
{ 
	{ "AMBIENT_LIGHT", LightModels::AMBIENT_LIGHT },
	{ "POINT_LIGHT", LightModels::POINT_LIGHT },
	{ "DIRECTIONAL_LIGHT", LightModels::DIRECTIONAL_LIGHT },
	{ "SPOT_LIGHT", LightModels::SPOT_LIGHT },
	{ "RIM_LIGHT", LightModels::RIM_LIGHT }
};

const std::vector<std::unique_ptr<LightType>> Light::LIGHT_APPLICATOR = Light::getLightTypeApplicators();
const std::vector<std::unique_ptr<LightAttenuation>> Light::LIGHT_ATTENUATION = Light::getLightAttenuationApplicators();
std::unordered_set<uint8_t> Light::ORTHOGRAPHIC_CAMERA{ DIRECTIONAL_LIGHT };

const vec3	Light::COLOR = vec3(0.4f, 0.4f, 0.4f);

const vec3	Light::POSITION = vec3(0.0f, 20.0f, 0.0f);
const vec3	Light::DIRECTION = vec3(0.0f, -1.0f, 0.0f);
const float Light::EXPONENT_S = 3.0f;
const float Light::COS_PENUMBRA = std::cos(glm::pi<float>() / 6.0f);
const float Light::COS_UMBRA = std::cos(glm::pi<float>() / 3.0f);

const float Light::C1 = 1.0f;
const float Light::C2 = 0.0f;
const float Light::C3 = 0.0f;
const float Light::MAX_DISTANCE = 20.0f;
const float Light::MIN_DISTANCE = 4.0f;
const float Light::FACTOR_C = 0.6f;
const float Light::DISTANCE_C = 10.0f;
const float Light::MAX_FACTOR = 0.3f;
const float Light::EXPONENT_SE = 3.0f;

const uint8_t	Light::BLUR_FILTER_SIZE = 5;
const float		Light::SHADOW_RADIUS = 0.005f;

/// [Public methods]

Light::Light()
{
	_lightType = POINT_LIGHT;

	_Ia = _Id = _Is = COLOR;
	_position = POSITION;
	_direction = DIRECTION;
	_exponentS = EXPONENT_S;
	_cosPenumbraAngle = COS_PENUMBRA; _cosUmbraAngle = COS_UMBRA;

	_attenuationType = DEFAULT_DISTANCE;

	_c1 = C1; _c2 = C2; _c3 = C3;
	_maxDistance = MAX_DISTANCE; _minDistance = MIN_DISTANCE;
	_factorC = FACTOR_C;
	_distC = DISTANCE_C;
	_maxAttFactor = MAX_FACTOR;
	_exponentSE = EXPONENT_SE;

	this->computePixarAttCoefficients();

	_enabled = true; 
	_castShadows = false; 

	const ivec2 canvasSize = Window::getInstance()->getSize();							
	_shadowMapCamera = new Camera(canvasSize.x, canvasSize.y);								// Some parameters from light can be translated to camera
	_shadowMapCamera->setPosition(_position);
	_shadowMapCamera->setLookAt(_position + _direction);
	_shadowMapCamera->setFovX(this->getUmbraDegrees());
	_shadowMapCamera->setCameraType((ORTHOGRAPHIC_CAMERA.find(_lightType) == ORTHOGRAPHIC_CAMERA.end()) ? Camera::PERSPECTIVE_PROJ : Camera::ORTHO_PROJ);

	_shadowMap = new ShadowMap(canvasSize.x, canvasSize.y);

	_shadowIntensity = vec2(0.0f, 1.0f);
	_shadowRadius = .005;
	_blurFilterSize = BLUR_FILTER_SIZE;
}

Light::~Light()
{
	delete _shadowMap;
	delete _shadowMapCamera;
}

void Light::applyLight(RenderingShader* shader, const mat4& viewMatrix, const GLuint shad1, const GLuint shad2)
{
	LIGHT_APPLICATOR[_lightType]->applyLight(this, shader, viewMatrix, shad1, shad2);
	LIGHT_ATTENUATION[_attenuationType]->applyAttenuation(this, shader);
}

void Light::applyLight4ColouredPoints(RenderingShader* shader, const mat4& viewMatrix)
{
	LIGHT_APPLICATOR[_lightType]->applyLight4ColouredPoints(this, shader, viewMatrix);
	LIGHT_ATTENUATION[_attenuationType]->applyAttenuation4ColouredPoints(this, shader);
}

void Light::applyShadowMapTexture(RenderingShader* shader, bool softShadows)
{
	if (this->_castShadows)
	{
		shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "depthTextureUniform", "shadow");

		if (!softShadows)
		{
			const float filterSize_2 = ((float)_blurFilterSize - 1) / 2.0f;
			const int minNeighborhood = std::ceil(-filterSize_2), maxNeighborhood = std::ceil(filterSize_2);

			shader->setUniform("minNeighborhood", minNeighborhood);
			shader->setUniform("maxNeighborhood", maxNeighborhood);
			shader->setUniform("neighborhoodSize", _blurFilterSize * _blurFilterSize);
		}
		else
		{
			shader->setUniform("shadowRadius", _shadowRadius);
		}

		shader->setUniform("shadowMaxIntensity", _shadowIntensity.y);
		shader->setUniform("shadowMinIntensity", _shadowIntensity.x);
		_shadowMap->applyTexture(shader);
	}
	else
	{
		shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "depthTextureUniform", "noShadow");
	}
}

void Light::applyShadowMapTexture4ColouredPoints(RenderingShader* shader)
{
	if (this->_castShadows)
	{
		shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "depthTextureUniform", "shadow");
		shader->setUniform("shadowMaxIntensity", _shadowIntensity.y);
		shader->setUniform("shadowMinIntensity", _shadowIntensity.x);
		_shadowMap->applyTexture(shader);
	}
	else
	{
		shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "depthTextureUniform", "noShadow");
	}
}

void Light::setLightType(const LightModels lightType)
{
	_lightType = lightType;
	_shadowMapCamera->setCameraType((ORTHOGRAPHIC_CAMERA.find(_lightType) == ORTHOGRAPHIC_CAMERA.end()) ? Camera::PERSPECTIVE_PROJ : Camera::ORTHO_PROJ);
}

void Light::setPosition(const vec3& position)
{
	_position = position;
	_shadowMapCamera->setPosition(position);
}

void Light::setDirection(const vec3& direction)
{
	_direction = glm::normalize(direction);
	_shadowMapCamera->setLookAt(_position + direction);
}

void Light::setExponentS(const float exponent)
{
	_exponentS = exponent;
}

void Light::setUmbraPenumbraRange(const float innerAngle, const float outterAngle)
{
	_cosPenumbraAngle = std::cos(innerAngle * glm::pi<float>() / 180.0f);
	_cosUmbraAngle = std::cos(outterAngle * glm::pi<float>() / 180.0f);
	_shadowMapCamera->setFovX(std::cos(this->getUmbraDegrees()));
}

void Light::setAttenuationCoefficients(const float c1, const float c2, const float c3)
{
	_c1 = c1; _c2 = c2; _c3 = c3;
}

void Light::setAttenuationDistanceRange(const float minDistance, const float maxDistance)
{
	_minDistance = minDistance;
	_maxDistance = glm::clamp(maxDistance, minDistance, FLT_MAX);
}

void Light::setAttenuationCoeffDistance(const float attFactor, const float distance)
{
	_factorC = attFactor; _distC = distance;
	this->computePixarAttCoefficients();
}

void Light::setMaxAttenuationFactor(const float maxAttFactor)
{
	_maxAttFactor = maxAttFactor;
	this->computePixarAttCoefficients();
}

void Light::setExponentSE(const float exponentSE)
{
	_exponentSE = exponentSE;
	this->computePixarAttCoefficients();
}

void Light::setBlurFilterSize(uint8_t size)
{
	_blurFilterSize = size;
}

void Light::setShadowIntensity(const float minIntensity, const float maxIntensity)
{
	_shadowIntensity = vec2(minIntensity, maxIntensity);
}

void Light::setShadowRadius(const float radius)
{
	_shadowRadius = radius;
}

Light::LightModels Light::stringToLightModel(const std::string& lightModel)
{
	auto it = LIGHT_ENUM_STR.find(lightModel);

	if (it != LIGHT_ENUM_STR.end())
	{
		return it->second;
	}

	return LightModels::DIRECTIONAL_LIGHT;
}

/// [Protected methods]

void Light::computePixarAttCoefficients()
{
	_k0 = std::log(_factorC / _maxAttFactor) / std::log(glm::e<float>());				// ln
	_k1 = _exponentSE / _k0;
}

std::vector<std::unique_ptr<LightAttenuation>> Light::getLightAttenuationApplicators()
{
	std::vector<std::unique_ptr<LightAttenuation>> applicator(PIXAR + 1);

	applicator[Light::DEFAULT_DISTANCE].reset(new BasicAttenuation());
	applicator[Light::CONTROLLED_DISTANCE].reset(new RangedAttenuation());
	applicator[Light::PIXAR].reset(new PixarAttenuation());

	return applicator;
}

std::vector<std::unique_ptr<LightType>> Light::getLightTypeApplicators()
{
	std::vector<std::unique_ptr<LightType>> applicator(RIM_LIGHT + 1);

	applicator[Light::AMBIENT_LIGHT].reset(new AmbientLight());
	applicator[Light::DIRECTIONAL_LIGHT].reset(new DirectionalLight());
	applicator[Light::POINT_LIGHT].reset(new PointLight());
	applicator[Light::RIM_LIGHT].reset(new RimLight());
	applicator[Light::SPOT_LIGHT].reset(new SpotLight());

	return applicator;
}

