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
