#include "stdafx.h"
#include "Material.h"

// [Static members initialization]

const float Material::DISP_FACTOR	= 0.1f;
const float Material::SHININESS		= 2.0f;

/// [Public methods]

Material::Material()
	: _displacementFactor(DISP_FACTOR), _shininess(SHININESS)
{
	for (int i = 0; i < Texture::NUM_TEXTURE_TYPES; ++i)
	{
		_texture[i] = nullptr;
	}
}

Material::Material(const Material& material)
{
	this->copyAttributes(material);
}

Material& Material::operator=(const Material& material)
{
	this->copyAttributes(material);

	return *this;
}

void Material::applyMaterial(RenderingShader* shader)
{
	if (_texture[Texture::KAD_TEXTURE])
	{
		_texture[Texture::KAD_TEXTURE]->applyTexture(shader, Texture::KAD_TEXTURE);
	}

	if (_texture[Texture::KS_TEXTURE])
	{
		shader->setUniform("shininess", _shininess);

		_texture[Texture::KS_TEXTURE]->applyTexture(shader, Texture::KS_TEXTURE);
	}

	if (_texture[Texture::SEMI_TRANSPARENT_TEXTURE])
	{
		shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "semiTransparentUniform", "semiTransparentTexture");

		_texture[Texture::SEMI_TRANSPARENT_TEXTURE]->applyTexture(shader, Texture::SEMI_TRANSPARENT_TEXTURE);
	}
	else
	{
		shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "semiTransparentUniform", "noSemiTransparentTexture");
	}

	if (_texture[Texture::BUMP_MAPPING_TEXTURE] && !_texture[Texture::DISPLACEMENT_MAPPING_TEXTURE])
	{
		shader->setSubroutineUniform(GL_VERTEX_SHADER, "displacementUniform", "bumpMappingDisplacement");
		shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "displacementUniform", "bumpMappingDisplacement");

		_texture[Texture::BUMP_MAPPING_TEXTURE]->applyTexture(shader, Texture::BUMP_MAPPING_TEXTURE);
	}
	else if (_texture[Texture::BUMP_MAPPING_TEXTURE] && _texture[Texture::DISPLACEMENT_MAPPING_TEXTURE])
	{
		shader->setSubroutineUniform(GL_VERTEX_SHADER, "displacementUniform", "displacementMapping");
		shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "displacementUniform", "displacementMapping");

		_texture[Texture::BUMP_MAPPING_TEXTURE]->applyTexture(shader, Texture::BUMP_MAPPING_TEXTURE);
	}
	else
	{
		shader->setSubroutineUniform(GL_VERTEX_SHADER, "displacementUniform", "noDisplacement");
		shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "displacementUniform", "noDisplacement");
	}

	shader->applyActiveSubroutines();
}

void Material::applyAlphaTexture(RenderingShader* shader)
{
	if (_texture[Texture::SEMI_TRANSPARENT_TEXTURE])
	{
		shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "semiTransparentUniform", "semiTransparentTexture");

		_texture[Texture::SEMI_TRANSPARENT_TEXTURE]->applyTexture(shader, Texture::SEMI_TRANSPARENT_TEXTURE);
	}
	else
	{
		shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "semiTransparentUniform", "noSemiTransparentTexture");
	}
}

void Material::applyMaterial4ColouredPoints(RenderingShader* shader)
{
	if (_texture[Texture::KAD_TEXTURE])
	{
		_texture[Texture::KAD_TEXTURE]->applyTexture(shader, Texture::KAD_TEXTURE);
	}

	if (_texture[Texture::KS_TEXTURE])
	{
		shader->setUniform("shininess", _shininess);

		_texture[Texture::KS_TEXTURE]->applyTexture(shader, Texture::KS_TEXTURE);
	}

	if (_texture[Texture::SEMI_TRANSPARENT_TEXTURE])
	{
		shader->setSubroutineUniform(GL_VERTEX_SHADER, "semiTransparentUniform", "semiTransparentTexture");

		_texture[Texture::SEMI_TRANSPARENT_TEXTURE]->applyTexture(shader, Texture::SEMI_TRANSPARENT_TEXTURE);
	}
	else
	{
		shader->setSubroutineUniform(GL_VERTEX_SHADER, "semiTransparentUniform", "noSemiTransparentTexture");
	}

	shader->applyActiveSubroutines();
}

void Material::applyMaterial4ComputeShader(ComputeShader* shader, bool applyBump)
{
	if (_texture[Texture::BUMP_MAPPING_TEXTURE] && applyBump)
	{
		_texture[Texture::BUMP_MAPPING_TEXTURE]->applyTexture(shader, Texture::BUMP_MAPPING_TEXTURE);
	}

	if (_texture[Texture::DISPLACEMENT_MAPPING_TEXTURE])
	{
		shader->setSubroutineUniform(GL_COMPUTE_SHADER, "displacementUniform", "displacementMapping");
		shader->setUniform("displacementLength", _displacementFactor);

		_texture[Texture::DISPLACEMENT_MAPPING_TEXTURE]->applyTexture(shader, Texture::DISPLACEMENT_MAPPING_TEXTURE);
	}
	else
	{
		shader->setSubroutineUniform(GL_COMPUTE_SHADER, "displacementUniform", "noDisplacement");
	}

	shader->applyActiveSubroutines();
}

void Material::applyMaterial4UniformColor(RenderingShader* shader)
{
	if (_texture[Texture::BUMP_MAPPING_TEXTURE] && !_texture[Texture::DISPLACEMENT_MAPPING_TEXTURE])
	{
		shader->setSubroutineUniform(GL_VERTEX_SHADER, "displacementUniform", "bumpMappingDisplacement");
		shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "displacementUniform", "bumpMappingDisplacement");

		_texture[Texture::BUMP_MAPPING_TEXTURE]->applyTexture(shader, Texture::BUMP_MAPPING_TEXTURE);
	}
	else if (_texture[Texture::BUMP_MAPPING_TEXTURE] && _texture[Texture::DISPLACEMENT_MAPPING_TEXTURE])
	{
		shader->setSubroutineUniform(GL_VERTEX_SHADER, "displacementUniform", "displacementMapping");
		shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "displacementUniform", "displacementMapping");

		_texture[Texture::BUMP_MAPPING_TEXTURE]->applyTexture(shader, Texture::BUMP_MAPPING_TEXTURE);
	}
	else
	{
		shader->setSubroutineUniform(GL_VERTEX_SHADER, "displacementUniform", "noDisplacement");
		shader->setSubroutineUniform(GL_FRAGMENT_SHADER, "displacementUniform", "noDisplacement");
	}

	shader->applyActiveSubroutines();
}

void Material::applyTexture(ShaderProgram* shader, const Texture::TextureTypes textureType)
{
	if (_texture[textureType])
	{
		_texture[textureType]->applyTexture(shader, textureType);
	}
}

void Material::setDisplacementFactor(const float dispFactor)
{
	_displacementFactor = dispFactor;
}

void Material::setShininess(const float shininess)
{
	_shininess = shininess;
}

void Material::setTexture(const Texture::TextureTypes textureType, Texture* texture)
{
	_texture[textureType] = texture;
}

/// [Protected methods]

void Material::copyAttributes(const Material& material)
{
	memcpy(&_texture, &material._texture, sizeof(material._texture));

	this->_displacementFactor = material._displacementFactor;
	this->_shininess = material._shininess;
}
