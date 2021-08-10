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
		TEXTURE_YELLOW
	};

	/**
	*	@return Number of built-in materials.
	*/
	const static GLsizei numAvailableMaterials() { return MATERIAL_CAD_WHITE + 1; }

	/**
	*	@return Number of built-in textures.
	*/
	const static GLsizei numAvailableTextures() { return TEXTURE_YELLOW + 1; }
};