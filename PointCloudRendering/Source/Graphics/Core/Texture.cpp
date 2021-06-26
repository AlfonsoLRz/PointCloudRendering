#include "stdafx.h"
#include "Texture.h"

// [Static variables initialization]

const GLuint Texture::MAG_FILTER = GL_LINEAR;
const GLuint Texture::MIN_FILTER = GL_LINEAR_MIPMAP_NEAREST;
const GLuint Texture::WRAP_S = GL_MIRRORED_REPEAT;
const GLuint Texture::WRAP_T = GL_MIRRORED_REPEAT;
const GLuint Texture::WRAP_R = GL_MIRRORED_REPEAT;

const std::unordered_map<uint16_t, std::string> Texture::SHADER_VARIABLE = std::unordered_map<uint16_t, std::string> ({
		{KAD_TEXTURE, "texKadSampler"},
		{KS_TEXTURE, "texKsSampler"},
		{SEMI_TRANSPARENT_TEXTURE, "texSemiTransparentSampler"},
		{BUMP_MAPPING_TEXTURE, "texBumpSampler"},
		{DISPLACEMENT_MAPPING_TEXTURE, "texDisplacementSampler"},
		{CUBEMAP_TEXTURE, "texCubeMapSampler"}
});

/// [Public methods]

/** glTexImage2D:
* Target. Type of desired texture: GL_TEXTURE_2D, GL_PROXY_TEXTURE_2D...
* Level:  Level of Detail. Level 0 is the base image. Level n is n-reduced image with mipmap algorithm
* Internal Format:  Number of texture components
* Width
* Height
* Border: must be 0
* Format: Format of pixel information. GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_RED_INTEGER...
* Type: Type of data of pixel information. GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT...
* Data: Image pixels
*/

Texture::Texture(const std::string& filename, const GLuint wrapS, const GLuint wrapT, const GLuint minFilter, const GLuint magFilter)
	: _id(-1)
{
	Image image (filename);
	this->loadImage(&image, minFilter, magFilter, wrapS, wrapT);
}

Texture::Texture(Image* image, const GLuint wrapS, const GLuint wrapT, const GLuint minFilter, const GLuint magFilter)
	: _id(-1)
{
	this->loadImage(image, minFilter, magFilter, wrapS, wrapT);
}

Texture::Texture(float* image, const int width, const int height, const GLuint wrapS, const GLuint wrapT, const GLuint minFilter, const GLuint magFilter, const bool rgba)
	: _id(-1)
{
	glGenTextures(1, &_id);
	glBindTexture(GL_TEXTURE_2D, _id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

	if (!rgba)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, image);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, image);
	}
	glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::Texture(std::vector<Image*> images, const int width, const int height, const GLuint wrapS, const GLuint wrapT, const GLuint wrapR, const GLuint minFilter, const GLuint magFilter)
	: _id(-1)
{
	glGenTextures(1, &_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _id);

	for (int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, images[i]->getWidth(), images[i]->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, images[i]->bits());
	}

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapT);
}

Texture::Texture(const vec4& color)
	: _id(-1)
{
	this->loadColor(color);
}

Texture::~Texture()
{
	glDeleteTextures(1, &_id);
}

void Texture::applyCubeMap(ShaderProgram* shader)
{
	const int textureID = CUBEMAP_TEXTURE;

	shader->setUniform(SHADER_VARIABLE.at(textureID), textureID);
	glActiveTexture(GL_TEXTURE0 + textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _id);
}

void Texture::applyTexture(ShaderProgram* shader, const TextureTypes textureType)
{
	const int textureID = textureType;

	shader->setUniform(SHADER_VARIABLE.at(textureID), textureID);
	glActiveTexture(GL_TEXTURE0 + textureID);
	glBindTexture(GL_TEXTURE_2D, _id);
}

void Texture::applyTexture(ShaderProgram* shader, const GLint id, const std::string& shaderVariable)
{
	shader->setUniform(shaderVariable, id);
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_2D, _id);
}

void Texture::applyTexture4ComputeShader(ComputeShader* shader, const GLint id, const std::string& shaderVariable, const GLuint readWrite, const GLuint format)
{
	glBindImageTexture(id, _id, 0, false, 0, readWrite, format);
	shader->setImageUniform(id, shaderVariable);
}

/// [Protected methods]

void Texture::loadColor(const vec4& color)
{
	const int width = 1, height = 1;
	const unsigned char image[] = { 255.0f * color.x, 255.0f * color.y, 255.0f * color.z, 255.0f * color.a };

	glGenTextures(1, &_id);
	glBindTexture(GL_TEXTURE_2D, _id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MIN_FILTER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MAG_FILTER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WRAP_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WRAP_T);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
}

bool Texture::loadImage(Image* image, const GLuint minFilter, const GLuint magFilter, const GLuint wrapS, const GLuint wrapT)
{
	unsigned char* bits = image->bits();
	if (!bits) 
	{
		return false;
	}

	const unsigned int width = image->getWidth(), height = image->getHeight();
	const GLuint depthID[] = { GL_RED, GL_RED, GL_RGB, GL_RGBA };

	glGenTextures(1, &_id);
	glBindTexture(GL_TEXTURE_2D, _id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

	glTexImage2D(GL_TEXTURE_2D, 0, depthID[image->getDepth() - 1], width, height, 0, depthID[image->getDepth() - 1], GL_UNSIGNED_BYTE, bits);
	glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}
