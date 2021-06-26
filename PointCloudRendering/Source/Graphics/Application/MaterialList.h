#pragma once

#include "Graphics/Application/GraphicsAppEnumerations.h"
#include "Graphics/Core/Material.h"
#include "Utilities/Singleton.h"

/**
*	@file MaterialList.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/19/2019
*/

/**
*	@brief Database of built-in application materials.
*/
class MaterialList: public Singleton<MaterialList>
{
	friend class Singleton<MaterialList>;

protected:
	std::vector<Material::MaterialSpecs>	_materialSpecs;				//!< Materials details so we can instantiate it only when needed
	std::vector<std::unique_ptr<Material>>	_material;					//!< Loaded materials									

protected:
	/**
	*	@brief Default constructor.
	*/
	MaterialList();

	/**
	*	@brief Creates a material once it's required.
	*/
	void createMaterial(const CGAppEnum::MaterialNames material);

	/**
	*	@brief Defines the content of each material.
	*/
	void defineMaterials();

public:
	/**
	*	@return Material defined by the index material (MaterialNames).
	*/
	Material* getMaterial(const CGAppEnum::MaterialNames material);
};

