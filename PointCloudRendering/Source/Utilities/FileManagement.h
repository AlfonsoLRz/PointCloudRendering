#pragma once

#include "stdafx.h"

#include "tinyply//tinyply.h"
#include "Graphics/Core/Group3D.h"

/**
*	@file FileManagement.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 08/07/2019
*/

/**
*	@brief Set of useful operations for file management.
*/
namespace FileManagement
{
	/**
	*	@brief
	*/
	void clearTokens(std::vector<std::string>& stringTokens, std::vector<float>& floatTokens);

	/**
	*	@brief Opens an imagen from a system file.
	*/
	bool openImage(const std::string& filename, std::vector<unsigned char>* image, unsigned int& width, unsigned int& height);

	/**
	*	@brief
	*/
	void readTokens(const std::string& line, const char delimiter, std::vector<std::string>& stringTokens, std::vector<float>& floatTokens);

	/**
	*	@brief Saves an image (array of bytes) in a system file, given the string filename.
	*/
	bool saveImage(const std::string& filename, std::vector<GLubyte>* image, const unsigned int width, const unsigned int height);
};

inline void FileManagement::clearTokens(std::vector<std::string>& stringTokens, std::vector<float>& floatTokens)
{
	stringTokens.clear();
	floatTokens.clear();
}

inline bool FileManagement::openImage(const std::string& filename, std::vector<unsigned char>* image, unsigned int& width, unsigned int& height)
{
	unsigned error = lodepng::decode(*image, width, height, filename.c_str());

	return error == 0;
}

inline void FileManagement::readTokens(const std::string& line, const char delimiter, std::vector<std::string>& stringTokens, std::vector<float>& floatTokens)
{
	std::stringstream ss(line);
	std::string tokenString;
	float tokenFloat;

	while (std::getline(ss, tokenString, delimiter) && !tokenString.empty())
	{
		try
		{
			tokenFloat = std::stof(tokenString);
			floatTokens.push_back(tokenFloat);
		}
		catch (std::exception& exception)
		{
			stringTokens.push_back(tokenString);
		}
	}
}

inline bool FileManagement::saveImage(const std::string& filename, std::vector<GLubyte>* image, const unsigned int width, const unsigned int height)
{
	std::vector<unsigned char> result;
	unsigned error = lodepng::encode(result, *image, width, height);

	if (!error)
	{
		lodepng::save_file(result, filename);

		return true;
	}

	return false;
}