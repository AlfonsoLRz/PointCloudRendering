#include "stdafx.h"
#include "MaterialList.h"

#include "Graphics/Application/TextureList.h"

/// [Protected methods]

MaterialList::MaterialList()
	: _material(CGAppEnum::numAvailableMaterials()), _materialSpecs(CGAppEnum::numAvailableMaterials())
{
	this->defineMaterials();
}

void MaterialList::createMaterial(const CGAppEnum::MaterialNames material)
{
	Material* mat = new Material();

	for (int i = 0; i < Texture::NUM_TEXTURE_TYPES; ++i)
	{
		if (_materialSpecs[material]._texture[i] != -1)
		{
			mat->setTexture(static_cast<Texture::TextureTypes>(i), TextureList::getInstance()->getTexture(static_cast<CGAppEnum::TextureNames>(_materialSpecs[material]._texture[i])));
		}
	}
	mat->setDisplacementFactor(_materialSpecs[material]._displacement);
	mat->setShininess(_materialSpecs[material]._shininess);

	_material[material].reset(mat);
}

void MaterialList::defineMaterials()
{
	/// TERRAIN_SCENE
	_materialSpecs[CGAppEnum::MATERIAL_CLAY]._texture[Texture::KAD_TEXTURE] = CGAppEnum::TEXTURE_CLAY_ALBEDO;
	_materialSpecs[CGAppEnum::MATERIAL_CLAY]._texture[Texture::KS_TEXTURE]	= CGAppEnum::TEXTURE_GRAY_127;
	_materialSpecs[CGAppEnum::MATERIAL_CLAY]._shininess = 120.0f;

	_materialSpecs[CGAppEnum::MATERIAL_FIRE_TOWER]._texture[Texture::KAD_TEXTURE] = CGAppEnum::TEXTURE_FIRE_TOWER_ALBEDO;
	_materialSpecs[CGAppEnum::MATERIAL_FIRE_TOWER]._texture[Texture::KS_TEXTURE] = CGAppEnum::TEXTURE_GRAY_127;
	_materialSpecs[CGAppEnum::MATERIAL_FIRE_TOWER]._texture[Texture::BUMP_MAPPING_TEXTURE] = CGAppEnum::TEXTURE_FIRE_TOWER_NORMAL;
	_materialSpecs[CGAppEnum::MATERIAL_FIRE_TOWER]._shininess = 300.0f;

	_materialSpecs[CGAppEnum::MATERIAL_GRASS]._texture[Texture::KAD_TEXTURE] = CGAppEnum::TEXTURE_GRASS_ALBEDO;
	_materialSpecs[CGAppEnum::MATERIAL_GRASS]._texture[Texture::KS_TEXTURE]	= CGAppEnum::TEXTURE_GRAY_127;
	_materialSpecs[CGAppEnum::MATERIAL_GRASS]._shininess = 120.0f;

	_materialSpecs[CGAppEnum::MATERIAL_LEAVES_01]._texture[Texture::KAD_TEXTURE] = CGAppEnum::TEXTURE_LEAVES_01_ALBEDO;
	_materialSpecs[CGAppEnum::MATERIAL_LEAVES_01]._texture[Texture::KS_TEXTURE] = CGAppEnum::TEXTURE_GRAY_64;
	_materialSpecs[CGAppEnum::MATERIAL_LEAVES_01]._texture[Texture::BUMP_MAPPING_TEXTURE] = CGAppEnum::TEXTURE_LEAVES_01_NORMAL;
	_materialSpecs[CGAppEnum::MATERIAL_LEAVES_01]._texture[Texture::SEMI_TRANSPARENT_TEXTURE] = CGAppEnum::TEXTURE_LEAVES_01_ALPHA;
	_materialSpecs[CGAppEnum::MATERIAL_LEAVES_01]._shininess = 300.0f;

	_materialSpecs[CGAppEnum::MATERIAL_STONE]._texture[Texture::KAD_TEXTURE] = CGAppEnum::TEXTURE_STONE_ALBEDO;
	_materialSpecs[CGAppEnum::MATERIAL_STONE]._texture[Texture::KS_TEXTURE]	= CGAppEnum::TEXTURE_GRAY_192;
	_materialSpecs[CGAppEnum::MATERIAL_STONE]._shininess = 100.0f;

	_materialSpecs[CGAppEnum::MATERIAL_TRANSMISSION_TOWER]._texture[Texture::KAD_TEXTURE] = CGAppEnum::TEXTURE_TRANSMISSION_TOWER_ALBEDO;
	_materialSpecs[CGAppEnum::MATERIAL_TRANSMISSION_TOWER]._texture[Texture::KS_TEXTURE] = CGAppEnum::TEXTURE_TRANSMISSION_TOWER_SPECULAR;
	_materialSpecs[CGAppEnum::MATERIAL_TRANSMISSION_TOWER]._shininess = 50.0f;

	_materialSpecs[CGAppEnum::MATERIAL_TRUNK_01]._texture[Texture::KAD_TEXTURE] = CGAppEnum::TEXTURE_TRUNK_01_ALBEDO;
	_materialSpecs[CGAppEnum::MATERIAL_TRUNK_01]._texture[Texture::KS_TEXTURE] = CGAppEnum::TEXTURE_BLACK;
	_materialSpecs[CGAppEnum::MATERIAL_TRUNK_01]._texture[Texture::BUMP_MAPPING_TEXTURE] = CGAppEnum::TEXTURE_TRUNK_01_NORMAL;
	_materialSpecs[CGAppEnum::MATERIAL_TRUNK_01]._shininess = 249.019f;

	_materialSpecs[CGAppEnum::MATERIAL_WATER]._texture[Texture::KAD_TEXTURE] = CGAppEnum::TEXTURE_WATER_LAKE_ALBEDO;
	_materialSpecs[CGAppEnum::MATERIAL_WATER]._texture[Texture::KS_TEXTURE] = CGAppEnum::TEXTURE_GRAY_192;
	_materialSpecs[CGAppEnum::MATERIAL_WATER]._texture[Texture::BUMP_MAPPING_TEXTURE] = CGAppEnum::TEXTURE_WATER_DUDV_NORMAL_MAP;
	_materialSpecs[CGAppEnum::MATERIAL_WATER]._shininess = 80.0f;

	_materialSpecs[CGAppEnum::MATERIAL_WEED_PLANT]._texture[Texture::KAD_TEXTURE]	= CGAppEnum::TEXTURE_WEED_PLANT_ALBEDO;
	_materialSpecs[CGAppEnum::MATERIAL_WEED_PLANT]._texture[Texture::KS_TEXTURE]	= CGAppEnum::TEXTURE_GRAY_64;
	_materialSpecs[CGAppEnum::MATERIAL_WEED_PLANT]._shininess = 10.0f;

	// CAD scene
	_materialSpecs[CGAppEnum::MATERIAL_CAD_WHITE]._texture[Texture::KAD_TEXTURE] = CGAppEnum::TEXTURE_WHITE;
	_materialSpecs[CGAppEnum::MATERIAL_CAD_WHITE]._texture[Texture::KS_TEXTURE] = CGAppEnum::TEXTURE_BLACK;
	_materialSpecs[CGAppEnum::MATERIAL_CAD_WHITE]._shininess = 100.0f;
}

/// [Public methods]

Material* MaterialList::getMaterial(const CGAppEnum::MaterialNames material)
{
	if (!_material[material].get())
	{
		this->createMaterial(material);
	}

	return _material[material].get();
}
