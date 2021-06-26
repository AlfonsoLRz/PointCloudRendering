#pragma once

#include "stdafx.h"
#include "Graphics/Application/GraphicsAppEnumerations.h"

/**
*	@file TerrainParameters.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 13/02/2021
*/

#define LOW_GEOMETRY_PARAMETERS false

/**
*	@brief Wraps those parameters which defines the terrain aspect.
*/
struct TerrainParameters
{
public:
	
#if LOW_GEOMETRY_PARAMETERS
	// Terrain
	inline static float		TERRAIN_MAX_HEIGHT		= 14.0f;
	inline static vec2		TERRAIN_SIZE			= vec2(20.0f);
	inline static uvec2		TERRAIN_SUBDIVISIONS	= uvec2(200, 200);

	// Water
	inline static float		WATER_HEIGHT			= -4.0f;
	inline static uvec2		WATER_SUBDIVISIONS		= uvec2(100, 100);

	// Terrain noise
	inline static float		FREQUENCY				= 0.008f;
	inline static float		GAIN					= 0.5f;
	inline static float		LACUNARITY				= 2.0f;
	inline static GLuint	MAP_SIZE				= 320;
	inline static float		NORMAL_MAP_DEPTH		= 5.0f;
	inline static GLuint	OCTAVES					= 6;

	// Erosion
	inline static float		DEPOSIT_SPEED			= .1f;
	inline static float		ERODE_SPEED				= 0.3f;
	inline static GLuint	EROSION_BRUSH_RADIUS	= 3;
	inline static GLuint	NUM_EROSION_ITERATIONS	= 200000;
	inline static float		EVAPORATE_SPEED			= .01f;
	inline static float		GRAVITY					= 1.0f;
	inline static float		INERTIA					= 0.2f;
	inline static GLuint	MAX_LIFETIME			= 15;
	inline static float		MIN_SEDIMENT_CAPACITY	= .01f;
	inline static float		SEDIMENT_CAPACITY_FACTOR = 4.0f;
	inline static float		START_SPEED				= 1.0f;
	inline static float		START_WATER				= 1.0f;

	// Terrain Rendering
	inline static float		GRASS_TEXTURE_SCALE		= 20.0f;
	inline static float		ROCK_TEXTURE_SCALE		= 10.0f;
	inline static float		ROCK_WEIGHT_FACTOR		= .9f;
	inline static GLuint	TEXTURE_START_INDEX		= 15;

	// Grass
	inline static vec2		GRASS_HEIGHT_THRESHOLD	= vec2(.0f, .8f);
	inline static float		GRASS_HEIGHT_WEIGHT		= 1.0f;
	inline static GLuint	NUM_GRASS_SEEDS			= 200000;
	inline static float		GRASS_SLOPE_WEIGHT		= 20.0f;
	inline static GLuint	GRASS_TEXTURE_SIZE		= 512;
	inline static float		GRASS_THRESHOLD			= 0.55f;
	inline static vec2		GRASS_SCALE				= vec2(.002f, .006f);

	// Forest
	inline static std::vector<CGAppEnum::MaterialNames>	CANOPY_MATERIALS		= { CGAppEnum::MATERIAL_LEAVES_01, CGAppEnum::MATERIAL_LEAVES_01, CGAppEnum::MATERIAL_LEAVES_01 };
	inline static std::vector<bool>						TREE_HAS_LEAVES			= { true, true, true };
	inline static std::vector<float>					MAX_TREE_SCALE			= { .095f, .085f, .055f };
	inline static std::vector<float>					MIN_TREE_SCALE			= { .05f, .04f, .045f };
	inline static unsigned								NUM_TREES				= 1000;
	inline static float									TREE_VERTICAL_OFFSET	= 0.03f;
	inline static std::vector<unsigned>					TREES_MODEL				= { 400, 350, 250 };
	inline static std::vector<std::string>				TREE_PATH				= { "Assets/Models/Trees/Tree01/", "Assets/Models/Trees/Tree02/", "Assets/Models/Trees/Tree03/" };
	inline static std::vector<CGAppEnum::MaterialNames>	TRUNK_MATERIALS			= { CGAppEnum::MATERIAL_TRUNK_01, CGAppEnum::MATERIAL_TRUNK_01, CGAppEnum::MATERIAL_TRUNK_01 };

	// Terrain Models
	inline static unsigned	MODEL_NUM_PARTICLES		= 5000;
	inline static unsigned	MODEL_RADIUS			= 4;
	inline static unsigned	MODEL_SATURATION_LEVEL	= 10;
	
#else
	// Terrain
	inline static float		TERRAIN_MAX_HEIGHT		= 14.0f;
	inline static vec2		TERRAIN_SIZE			= vec2(20.0f);
	inline static uvec2		TERRAIN_SUBDIVISIONS	= uvec2(200, 200);

	// Water
	inline static float		WATER_HEIGHT			= -4.0f;
	inline static uvec2		WATER_SUBDIVISIONS		= uvec2(100, 100);

	// Terrain noise
	inline static float		FREQUENCY				= 0.008f;
	inline static float		GAIN					= 0.5f;
	inline static float		LACUNARITY				= 2.0f;
	inline static GLuint	MAP_SIZE				= 320;
	inline static float		NORMAL_MAP_DEPTH		= 5.0f;
	inline static GLuint	OCTAVES					= 6;

	// Erosion
	inline static float		DEPOSIT_SPEED			= .1f;
	inline static float		ERODE_SPEED				= 0.3f;
	inline static GLuint	EROSION_BRUSH_RADIUS	= 3;
	inline static GLuint	NUM_EROSION_ITERATIONS	= 200000;
	inline static float		EVAPORATE_SPEED			= .01f;
	inline static float		GRAVITY					= 1.0f;
	inline static float		INERTIA					= 0.2f;
	inline static GLuint	MAX_LIFETIME			= 15;
	inline static float		MIN_SEDIMENT_CAPACITY	= .01f;
	inline static float		SEDIMENT_CAPACITY_FACTOR = 4.0f;
	inline static float		START_SPEED				= 1.0f;
	inline static float		START_WATER				= 1.0f;

	// Terrain Rendering
	inline static float		GRASS_TEXTURE_SCALE		= 20.0f;
	inline static float		ROCK_TEXTURE_SCALE		= 10.0f;
	inline static float		ROCK_WEIGHT_FACTOR		= .9f;
	inline static GLuint	TEXTURE_START_INDEX		= 15;

	// Grass
	inline static vec2		GRASS_HEIGHT_THRESHOLD	= vec2(.0f, .8f);
	inline static float		GRASS_HEIGHT_WEIGHT		= 1.0f;
	inline static GLuint	NUM_GRASS_SEEDS			= 800000;
	inline static float		GRASS_SLOPE_WEIGHT		= 20.0f;
	inline static GLuint	GRASS_TEXTURE_SIZE		= 512;
	inline static float		GRASS_THRESHOLD			= 0.55f;
	inline static vec2		GRASS_SCALE				= vec2(.002f, .006f);

	// Forest
	inline static std::vector<CGAppEnum::MaterialNames>	CANOPY_MATERIALS		= { CGAppEnum::MATERIAL_LEAVES_01, CGAppEnum::MATERIAL_LEAVES_01, CGAppEnum::MATERIAL_LEAVES_01 };
	inline static std::vector<bool>						TREE_HAS_LEAVES			= { true, true, true };
	inline static std::vector<float>					MAX_TREE_SCALE			= { .095f, .085f, .055f };
	inline static std::vector<float>					MIN_TREE_SCALE			= { .05f, .04f, .045f };
	inline static unsigned								NUM_TREES				= 2500;
	inline static float									TREE_VERTICAL_OFFSET	= 0.03f;
	inline static std::vector<unsigned>					TREES_MODEL				= { 1500, 750, 250 };
	inline static std::vector<std::string>				TREE_PATH				= { "Assets/Models/Trees/Tree01/", "Assets/Models/Trees/Tree02/", "Assets/Models/Trees/Tree03/" };
	inline static std::vector<CGAppEnum::MaterialNames>	TRUNK_MATERIALS			= { CGAppEnum::MATERIAL_TRUNK_01, CGAppEnum::MATERIAL_TRUNK_01, CGAppEnum::MATERIAL_TRUNK_01 };

	// Terrain Models
	inline static unsigned	MODEL_NUM_PARTICLES		= 5000;
	inline static unsigned	MODEL_RADIUS			= 4;
	inline static unsigned	MODEL_SATURATION_LEVEL	= 10;
#endif
	
	// Compute shaders
	inline static const float		FLOAT_MULTIPLIER	= 100000.0f;

	// Scene storage
	inline static const std::string BINARY_LOCATION = "Assets/Scene/TerrainEnvironment/BinaryDescription.bin";

	// Additional storage
	struct TerrainUIParameters
	{
		bool		_enableWatcher;
		bool		_enableTransmissionTower;
		float		_noiseFrequency;
		uvec2		_numTerrainSubdivisions;
		uvec2		_numWaterSubdivisions;
		GLuint		_numTrees[3];
		GLuint		_numGrassSeeds;
		vec2		_terrainSize;
		bool		_useSimplex;
	};

	// ---- Methods

	/**
	*	@brief Loads binary description of terrain, if exists, onto these parameters. 
	*/
	static void loadTerrainParameters();
};

inline void TerrainParameters::loadTerrainParameters()
{
	std::ifstream fin(BINARY_LOCATION, std::ios::in | std::ios::binary);
	if (!fin.is_open()) return;

	TerrainUIParameters parameters;
	fin.read((char*)&parameters, sizeof(TerrainUIParameters));
	fin.close();

	// Modify static parameters according to binary description
	{
		
	}
}

