#pragma once

/**
*	@file GraphicsAppEnumerations.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/12/2019
*/

/**
*	@brief Enumerations for any component of this application.
*/
struct CGAppEnum
{
	// Renderization modes
	enum VisualizationModes : uint8_t
	{
		VIS_POINTS,
		VIS_LINES,
		VIS_TRIANGLES,
		VIS_ALL_TOGETHER
	};

	// Drawable scenes
	enum SceneNames : uint8_t
	{
		TERRAIN_SCENE,
		CAD_SCENE
	};

	/**
	*	@return Number of different visualization modes.
	*/
	const static GLsizei numVisualizationModes() { return VIS_ALL_TOGETHER + 1; }

	/**
	*	@return Number of drawable scenes.
	*/
	const static GLsizei numAvailableScenes() { return CAD_SCENE + 1; }


	// Application prebuilt materials
	enum MaterialNames : uint16_t
	{
		// TERRAIN SCENE
		MATERIAL_CLAY,
		MATERIAL_FIRE_TOWER,
		MATERIAL_GRASS,
		MATERIAL_LEAVES_01,
		MATERIAL_STONE,
		MATERIAL_TRANSMISSION_TOWER,
		MATERIAL_TRUNK_01,
		MATERIAL_WATER,
		MATERIAL_WEED_PLANT,

		// CAD 
		MATERIAL_CAD_WHITE
	};

	// Application prebuilt textures
	enum TextureNames : uint16_t
	{
		// Colors for any purpose
		TEXTURE_BLUE,
		TEXTURE_BLUE_SKY,
		TEXTURE_BLACK,
		TEXTURE_GOLD,
		TEXTURE_GOLD_SHININESS,
		TEXTURE_GRAY_64,
		TEXTURE_GRAY_127,
		TEXTURE_GRAY_192, 
		TEXTURE_GREEN,
		TEXTURE_RED,
		TEXTURE_WHITE,
		TEXTURE_YELLOW,

		/// ----- TERRAIN SCENE -----

		// Albedo
		TEXTURE_CLAY_ALBEDO,
		TEXTURE_CRYSTAL_ALBEDO,
		TEXTURE_FIRE_TOWER_ALBEDO,
		TEXTURE_GRASS_ALBEDO,
		TEXTURE_LEAVES_01_ALBEDO,
		TEXTURE_STONE_ALBEDO,
		TEXTURE_TRANSMISSION_TOWER_ALBEDO,
		TEXTURE_TRUNK_01_ALBEDO,
		TEXTURE_TRUNK_02_ALBEDO,
		TEXTURE_WATER_LAKE_ALBEDO,
		TEXTURE_WEED_PLANT_ALBEDO,

		// Normal
		TEXTURE_CLAY_NORMAL,
		TEXTURE_FIRE_TOWER_NORMAL,
		TEXTURE_GRASS_NORMAL,
		TEXTURE_LEAVES_01_NORMAL,
		TEXTURE_TRANSMISSION_TOWER_NORMAL,
		TEXTURE_STONE_NORMAL,
		TEXTURE_TRUNK_01_NORMAL,

		// Displacement
		TEXTURE_CLAY_DISP,
		TEXTURE_GRASS_DISP,
		TEXTURE_STONE_DISP,

		// Alpha
		TEXTURE_LEAVES_01_ALPHA,

		// Others
		TEXTURE_LIDAR_HEIGHT,
		TEXTURE_WATER_DUDV_MAP,
		TEXTURE_WATER_DUDV_NORMAL_MAP,
		TEXTURE_TRANSMISSION_TOWER_SPECULAR,
		TEXTURE_TRUNK_01_SPECULAR
	};

	/**
	*	@return Number of built-in materials.
	*/
	const static GLsizei numAvailableMaterials() { return MATERIAL_CAD_WHITE + 1; }

	/**
	*	@return Number of built-in textures.
	*/
	const static GLsizei numAvailableTextures() { return TEXTURE_TRUNK_01_SPECULAR + 1; }
};