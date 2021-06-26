#pragma once

#include "Graphics/Application/GraphicsAppEnumerations.h"
#include "Graphics/Core/Texture.h"
#include "Utilities/Singleton.h"

/**
*	@file TextureList.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/19/2019
*/

/**
*	@brief Database of built-in application textures.
*/
class TextureList: public Singleton<TextureList>
{
	friend class Singleton<TextureList>;

protected:
	static std::unordered_map<uint16_t, std::string> TEXTURE_PATH;			//!< Location of every texture which is not defined by a single color

protected:
	std::vector<std::unique_ptr<Texture>> _texture;							//!< Only queried textures are initialized

protected:
	/**
	*	@brief Default constructor.
	*/
	TextureList();

	/**
	*	@brief Defines the composition of each texture.
	*/
	void loadTextures();

public:
	/**
	*	@return Texture defined by the index material (TextureNames).
	*/
	Texture* getTexture(const CGAppEnum::TextureNames texture);
};

