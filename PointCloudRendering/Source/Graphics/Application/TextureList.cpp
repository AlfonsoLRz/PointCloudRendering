#include "stdafx.h"
#include "TextureList.h"

// [Static members initialization]

std::unordered_map<uint16_t, std::string> TextureList::TEXTURE_PATH
{
	// TERRAIN_SCENE
	{CGAppEnum::TEXTURE_CLAY_ALBEDO, "Assets/Textures/Floor/Clay/Albedo.png"},
	{CGAppEnum::TEXTURE_CLAY_DISP, "Assets/Textures/Floor/Clay/Displacement.png"},
	{CGAppEnum::TEXTURE_CLAY_NORMAL, "Assets/Textures/Floor/Clay/Normal.png"},
	{CGAppEnum::TEXTURE_FIRE_TOWER_ALBEDO, "Assets/Textures/Buildings/Tower/Albedo.png"},
	{CGAppEnum::TEXTURE_FIRE_TOWER_NORMAL, "Assets/Textures/Buildings/Tower/Normal.png"},
	{CGAppEnum::TEXTURE_GRASS_ALBEDO, "Assets/Textures/Floor/Grass/Albedo.png"},
	{CGAppEnum::TEXTURE_GRASS_DISP, "Assets/Textures/Floor/Grass/Displacement.png"},
	{CGAppEnum::TEXTURE_GRASS_NORMAL, "Assets/Textures/Floor/Grass/Normal.png"},
	{CGAppEnum::TEXTURE_LEAVES_01_ALBEDO, "Assets/Textures/Trees/Leaf_Albedo.png"},
	{CGAppEnum::TEXTURE_LEAVES_01_ALPHA, "Assets/Textures/Trees/Leaf_Alpha.png"},
	{CGAppEnum::TEXTURE_LEAVES_01_NORMAL, "Assets/Textures/Trees/Leaf_Normal.png"},
	{CGAppEnum::TEXTURE_LIDAR_HEIGHT, "Assets/Textures/LiDAR/HeightMap.png"},
	{CGAppEnum::TEXTURE_STONE_ALBEDO, "Assets/Textures/Floor/Stone/Albedo.png"},
	{CGAppEnum::TEXTURE_STONE_DISP, "Assets/Textures/Floor/Stone/Displacement.png"},
	{CGAppEnum::TEXTURE_STONE_NORMAL, "Assets/Textures/Floor/Stone/Normal.png"},
	{CGAppEnum::TEXTURE_TRANSMISSION_TOWER_ALBEDO, "Assets/Textures/Buildings/TransmissionTower/MetalAlbedo.png"},
	{CGAppEnum::TEXTURE_TRANSMISSION_TOWER_NORMAL, "Assets/Textures/Buildings/TransmissionTower/MetalNormal.png"},
	{CGAppEnum::TEXTURE_TRANSMISSION_TOWER_SPECULAR, "Assets/Textures/Buildings/TransmissionTower/MetalRoughness.png"},
	{CGAppEnum::TEXTURE_TRUNK_01_ALBEDO, "Assets/Textures/Trees/Trunk_Albedo.png"},
	{CGAppEnum::TEXTURE_TRUNK_01_NORMAL, "Assets/Textures/Trees/Trunk_Normal.png"},
	{CGAppEnum::TEXTURE_TRUNK_01_SPECULAR, "Assets/Textures/Trees/Trunk_Specular.png"},
	{CGAppEnum::TEXTURE_WATER_DUDV_MAP, "Assets/Textures/Water/DuDvMap.png"},
	{CGAppEnum::TEXTURE_WATER_DUDV_NORMAL_MAP, "Assets/Textures/Water/DuDvNormalMap.png"},
};

/// [Protected methods]

TextureList::TextureList()
	: _texture(CGAppEnum::numAvailableTextures())
{
	this->loadTextures();
}

void TextureList::loadTextures()
{
	// Solution to increase flexibility of texture instantiation: define here only those textures which doesn't come from images in file system or use different parameters for
	// magnification / mignification or wrapping S / T 

	_texture[CGAppEnum::TEXTURE_BLACK]				= std::unique_ptr<Texture>(new Texture(vec4(0.0f, 0.0f, 0.0f, 1.0f)));
	_texture[CGAppEnum::TEXTURE_BLUE]				= std::unique_ptr<Texture>(new Texture(vec4(0.35f, 0.35f, 1.0f, 1.0f)));
	_texture[CGAppEnum::TEXTURE_BLUE_SKY]			= std::unique_ptr<Texture>(new Texture(vec4(0.04f, 0.7f, 0.94f, 1.0f)));
	_texture[CGAppEnum::TEXTURE_CRYSTAL_ALBEDO]		= std::unique_ptr<Texture>(new Texture(vec4(0.0f, 0.0f, 0.05f, 0.1f)));
	_texture[CGAppEnum::TEXTURE_GRAY_64]			= std::unique_ptr<Texture>(new Texture(vec4(0.25f, 0.25f, 0.25f, 1.0f)));
	_texture[CGAppEnum::TEXTURE_GRAY_127]			= std::unique_ptr<Texture>(new Texture(vec4(0.5f, 0.5f, 0.5f, 1.0f)));
	_texture[CGAppEnum::TEXTURE_GRAY_192]			= std::unique_ptr<Texture>(new Texture(vec4(0.75f, 0.75f, 0.75f, 1.0f)));
	_texture[CGAppEnum::TEXTURE_GOLD]				= std::unique_ptr<Texture>(new Texture(vec4(0.751f, 0.606f, 0.226f, 1.0f)));
	_texture[CGAppEnum::TEXTURE_GOLD_SHININESS]		= std::unique_ptr<Texture>(new Texture(vec4(0.628f, 0.555f, 0.366f, 1.0f)));
	_texture[CGAppEnum::TEXTURE_GREEN]				= std::unique_ptr<Texture>(new Texture(vec4(0.0f, 1.0f, 0.0f, 1.0f)));
	_texture[CGAppEnum::TEXTURE_RED]				= std::unique_ptr<Texture>(new Texture(vec4(1.0f, 0.0f, 0.0f, 1.0f)));
	_texture[CGAppEnum::TEXTURE_WATER_LAKE_ALBEDO]	= std::unique_ptr<Texture>(new Texture(vec4(0.45f, 0.48f, 0.5f, 0.46f)));
	_texture[CGAppEnum::TEXTURE_WEED_PLANT_ALBEDO]	= std::unique_ptr<Texture>(new Texture(vec4(0.2f, 0.42f, 0.1f, 1.0f)));
	_texture[CGAppEnum::TEXTURE_WHITE]				= std::unique_ptr<Texture>(new Texture(vec4(1.0f, 1.0f, 1.0f, 1.0f)));
	_texture[CGAppEnum::TEXTURE_YELLOW]				= std::unique_ptr<Texture>(new Texture(vec4(1.0f, 1.0f, 0.0f, 1.0f)));
}

/// [Public methods]

Texture* TextureList::getTexture(const CGAppEnum::TextureNames texture)
{
	if (!_texture[texture].get())
	{
		if (TEXTURE_PATH.find(texture) == TEXTURE_PATH.end())
		{
			return nullptr;
		}

		const std::string texturePath = TEXTURE_PATH[texture];
		Texture* text = new Texture(texturePath);

		_texture[texture].reset(text);
	}

	return _texture[texture].get();
}
