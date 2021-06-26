#pragma once

#include "Graphics/Core/Camera.h"
#include "Graphics/Core/RenderingShader.h"
#include "Graphics/Core/ShadowMap.h"

/**
*	@file Light.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/11/2019
*/

class LightType;
class LightAttenuation;

#define ENUM_TO_STR(ENUM) std::string(#ENUM)

/**
*	@brief Generic light which implements the Strategy pattern instead of declaring several light types.
*/
class Light
{
public:
	// [Strategy pattern]
	enum LightModels: uint8_t
	{
		AMBIENT_LIGHT, POINT_LIGHT, DIRECTIONAL_LIGHT, SPOT_LIGHT, RIM_LIGHT
	};

	enum AttenuationTypes: uint8_t
	{
		DEFAULT_DISTANCE, CONTROLLED_DISTANCE, PIXAR
	};

protected:
	const static std::unordered_map<std::string, LightModels> LIGHT_ENUM_STR;

	// [Strategy pattern]
	const static std::vector<std::unique_ptr<LightType>> LIGHT_APPLICATOR;							//!< Applicators for each light type
	const static std::vector<std::unique_ptr<LightAttenuation>> LIGHT_ATTENUATION;					//!< Applicators for each attenuation type

	// [Shadow projection data]
	static std::unordered_set<uint8_t> ORTHOGRAPHIC_CAMERA;											//!< Light types which needs an orthographic camera to produce shadows

protected:
	// [Fixed values: by default initialization]

	const static vec3		COLOR;

	const static vec3		POSITION, DIRECTION;
	const static float		EXPONENT_S, COS_PENUMBRA, COS_UMBRA;

	const static float		C1, C2, C3;
	const static float		MAX_DISTANCE, MIN_DISTANCE;
	const static float		FACTOR_C, DISTANCE_C, MAX_FACTOR, EXPONENT_SE;

	const static uint8_t	BLUR_FILTER_SIZE;
	const static float		SHADOW_RADIUS;

protected:
	// [Colors]
	vec3		_Ia, _Id, _Is;						//!< Ambient (as well as rim), diffuse and specular colors!

	// [Spatial parameters]
	vec3		_position;							//!< World position
	vec3		_direction;							//!< Only for directional and spot lights
	float		_exponentS;							//!< Phong exponent
	float		_cosPenumbraAngle;					//!< Boundary angle which delimites the zone completely lighted (attenuation = 1) and the fading area (radians)
	float		_cosUmbraAngle;						//!< Boundary angle to attenuate completely the surface color (radians)

	// [Attenuation]
	float		_c1, _c2, _c3;						//!< Attenuation factor													(Model 1)
	float		_minDistance, _maxDistance;			//!< Mininum distance -> max distance (attenuation range)				(Model 2)
	float		_factorC, _distC;					//!< Attenuation factor factorC is reached at distC distance			(Model 3: Pixar)
	float		_maxAttFactor;						//!< Maximum attenuation factor											(Model 3: Pixar)
	float		_exponentSE;						//!< Decreases attenuation factor as the distance gets bigger			(Model 3: Pixar)
	float		_k0, _k1;							//!< Derivated attributes from previous variables						(Model 3: Pixar)

	// [State]
	bool		_enabled;							//!< Light on / off
	bool		_castShadows;						//!< Produces shadows
	uint8_t		_lightType;							//!< Active light model
	uint8_t		_attenuationType;					//!< Active attenuation model

	// [Cast shadows]
	uint8_t		_blurFilterSize;					//!< Determines the amount of neighboors which are taken into account to smooth the shadow
	vec2		_shadowIntensity;					//!< Boundaries for shadow intensity! Let us control the intensity of those areas which are shadowed
	Camera*		_shadowMapCamera;					//!< Camera with its eye at the light position
	ShadowMap*	_shadowMap;							//!< Shadow map rendered from the shadow map camera
	float		_shadowRadius;						//!< Shadow radius from where offsets are considered

protected:
	/**
	*	@brief Computes the coefficientes K0 and K1 from the pixar attenuation formula.
	*/
	void computePixarAttCoefficients();

	/**
	*	@return Array with applicators for each attenuation type.
	*/
	static std::vector<std::unique_ptr<LightAttenuation>> getLightAttenuationApplicators();

	/**
	*	@return Array with applicators for each light type.
	*/
	static std::vector<std::unique_ptr<LightType>> getLightTypeApplicators();

public:
	/**
	*	@brief Default constructor.
	*/
	Light();

	/**
	*	@brief Destructor.
	*/
	~Light();

	/**
	*	@brief Applies the uniforms variables related to the light.
	*	@param shader Shader where we need to specify the uniform variables.
	*	@param viewMatrix View matrix of currently active camera.
	*/
	void applyLight(RenderingShader* shader, const mat4& viewMatrix, const GLuint shad1 = GL_VERTEX_SHADER, const GLuint shad2 = GL_FRAGMENT_SHADER);

	/**
	*	@brief Applies the uniforms variables related to the light.
	*	@param shader Shader where we need to specify the uniform variables.
	*	@param viewMatrix View matrix of currently active camera.
	*/
	void applyLight4ColouredPoints(RenderingShader* shader, const mat4& viewMatrix);

	/**
	*	@brief Specifies the uniform variables related to shadow mapping.
	*	@param shader Shader where those variables must be linked.
	*/
	void applyShadowMapTexture(RenderingShader* shader, bool softShadows = true);

	/**
	*	@brief Specifies the uniform variables related to shadow mapping.
	*	@param shader Shader where those variables must be linked.
	*/
	void applyShadowMapTexture4ColouredPoints(RenderingShader* shader);

	/**
	*	@brief Enables or disables the shadow projection from this light source.
	*/
	void castShadows(const bool enabled) { _castShadows = enabled; }

	/**
	*	@brief Light isn't enabled and must not be taken into account when rendering the scene.
	*/
	void disable() { _enabled = false; }

	/**
	*	@brief Light is on when rendering scene.
	*/
	void enable() { _enabled = true; }

	/**
	*	@return Index of attenuation formula.
	*/
	AttenuationTypes getAttenuationType() const { return static_cast<AttenuationTypes>(_attenuationType); }

	/**
	*	@return Index of type of light.
	*/
	LightModels getLightType() const { return static_cast<LightModels>(_lightType); }

	/**
	*	@return True boolean value if the light is activated.	
	*/
	bool isEnabled() { return _enabled; }

	/**
	*	@brief Modifies the attenuation which is applied to the light.
	*/
	void setAttenuationType(const AttenuationTypes attType) { _attenuationType = attType; }

	/**
	*	@brief Modifies the type of light and how it affects the scene.
	*/
	void setLightType(const LightModels lightType);

	/**
	*	@return True if the light should cast shadows.
	*/
	bool shouldCastShadows() { return _castShadows; }

	/// [Getters]

	/**
	*	@return Ambient color.
	*/
	vec3 getIa() const { return _Ia; }

	/**
	*	@return Diffuse color.
	*/
	vec3 getId() const { return _Id; }

	/**
	*	@return Specular color.
	*/
	vec3 getIs() const { return _Is; }

	/**
	*	@return Position in world.
	*/
	vec3 getPosition() const { return _position; }

	/**
	*	@return Lighting direction, if ligth is either directional or spot.
	*/
	vec3 getDirection() const { return _direction; }

	/**
	*	@return Phong exponent.
	*/
	float getExponentS() const { return _exponentS; }

	/**
	*	@return Angle, in degrees, which marks the boundary between fading out area and lighted area.
	*/
	float getPenumbraDegrees() const { return glm::degrees(std::acos(_cosPenumbraAngle)); }

	/**
	*	@return Angle, in degrees, which marks the boundary between fading out area and dark area.
	*/
	float getUmbraDegrees() const { return glm::degrees(std::acos(_cosUmbraAngle)); }

	/**
	*	@return Attenuation coefficients for basic formula. From 0 to 2: c1, c2, c3, where c3 is related to the squared radius.
	*/
	vec3 getAttCoefficients() const { return vec3(_c1, _c2, _c3); }

	/**
	*	@return Distance range (minimum -> x, maximum -> y) for distance formula.
	*/
	vec2 getDistanceRange() const { return vec2(_minDistance, _maxDistance); }

	/**
	*	@return Attenuation factor C in Pixar formula.
	*/
	float getFactorC() const { return _factorC; }

	/**
	*	@return Distance where factorC attenuation value is reached.
	*/
	float getDistanceC() const { return _distC; }

	/**
	*	@return Maximum attenuation factor.
	*/
	float getMaxAttFactor() const { return _maxAttFactor; }

	/**
	*	@return Exponent SE for Pixar formula.
	*/
	float getExponentSE() const { return _exponentSE; }

	/**
	*	@return K0 and K1 coefficients from Pixar attenuation formula.
	*/
	vec2 getKCoefficients() const { return vec2(_k0, _k1); }

	/**
	*	@return Camera which is used to cast shadows.
	*/
	Camera* getCamera() const { return _shadowMapCamera; }

	/**
	*	@brief Shadow map where the depth of any scene is rendered.
	*/
	ShadowMap* getShadowMap() const { return _shadowMap; }

	/// [Setters]

	/**
	*	@brief Modifies the ambient light (also the color for rim lights).
	*/
	void setIa(const vec3& Ia) { _Ia = Ia; }

	/**
	*	@brief Modifies the color of the diffuse term.
	*/
	void setId(const vec3& Id) { _Id = Id; }

	/**
	*	@brief Modifies the color of the specular term.
	*/
	void setIs(const vec3& Is) { _Is = Is; }

	/**
	*	@brief Modifies the position from where the light begins.
	*/
	void setPosition(const vec3& position);

	/**
	*	@brief Modifies where a directional light looks at.
	*/
	void setDirection(const vec3& direction);

	/**
	*	@brief Modifies the Phong exponent.
	*/
	void setExponentS(const float exponent);

	/**
	*	@brief Modifies the circles where the light disappear [outterAngle] and fades out [innerAngle].
	*	@param innerAngle Angle whick marks the fading out and lighted area (degrees).
	*	@param outterAngle Angle which marks the fading out and dark area (degrees).
	*/
	void setUmbraPenumbraRange(const float innerAngle, const float outterAngle);

	/**
	*	@brief Modifies the attenuation coefficient of the basic formula.
	*/
	void setAttenuationCoefficients(const float c1, const float c2, const float c3);

	/**
	*	@brief Modifies the attenuation distances for the second formula.
	*/
	void setAttenuationDistanceRange(const float minDistance, const float maxDistance);

	/**
	*	@brief Modifies the attenuation factor at a certain distance for the Pixar formula.
	*/
	void setAttenuationCoeffDistance(const float attFactor, const float distance);

	/**
	*	@brief Modifies the maximum attenuation which reaches the Pixar attenuation formula.
	*/
	void setMaxAttenuationFactor(const float maxAttFactor);

	/**
	*	@brief Modifies the exponent for the Pixar attenuation formula.
	*/
	void setExponentSE(const float exponentSE);

	/**
	*	@brief Modifies the size of blur filter which allows us to smooth the shadow.
	*	@param size Neighborhood size.
	*/
	void setBlurFilterSize(uint8_t size);

	/**
	*	@brief Modifies the intensity range which a shadow generated by this light source can take.
	*	@param minIntensity Value in [0, 1].
	*	@param maxIntensity Value in [0, 1], greater than minIntensity.
	*/
	void setShadowIntensity(const float minIntensity, const float maxIntensity);

	/**
	*	@brief Modifies the radius which is considered to calculate the shadow factor.
	*/
	void setShadowRadius(const float radius);

	/**
	*	@return LightModel enum value which corresponds to the string in parameters.
	*/
	static LightModels stringToLightModel(const std::string& lightModel);
};

