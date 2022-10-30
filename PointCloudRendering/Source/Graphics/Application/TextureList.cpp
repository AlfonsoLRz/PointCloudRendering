#include "stdafx.h"
#include "TextureList.h"

// [Static members initialization]

std::unordered_map<uint16_t, std::string> TextureList::TEXTURE_PATH
{
	// TERRAIN_SCENE
	{CGAppEnum::TEXTURE_INFERNO, "Assets/Textures/Gradient/Inferno.png"}
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
	_texture[CGAppEnum::TEXTURE_GRAY_64]			= std::unique_ptr<Texture>(new Texture(vec4(0.25f, 0.25f, 0.25f, 1.0f)));
	_texture[CGAppEnum::TEXTURE_GRAY_127]			= std::unique_ptr<Texture>(new Texture(vec4(0.5f, 0.5f, 0.5f, 1.0f)));
	_texture[CGAppEnum::TEXTURE_GRAY_192]			= std::unique_ptr<Texture>(new Texture(vec4(0.75f, 0.75f, 0.75f, 1.0f)));
	_texture[CGAppEnum::TEXTURE_GOLD]				= std::unique_ptr<Texture>(new Texture(vec4(0.751f, 0.606f, 0.226f, 1.0f)));
	_texture[CGAppEnum::TEXTURE_GOLD_SHININESS]		= std::unique_ptr<Texture>(new Texture(vec4(0.628f, 0.555f, 0.366f, 1.0f)));
	_texture[CGAppEnum::TEXTURE_GREEN]				= std::unique_ptr<Texture>(new Texture(vec4(0.0f, 1.0f, 0.0f, 1.0f)));
	_texture[CGAppEnum::TEXTURE_RED]				= std::unique_ptr<Texture>(new Texture(vec4(1.0f, 0.0f, 0.0f, 1.0f)));
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
