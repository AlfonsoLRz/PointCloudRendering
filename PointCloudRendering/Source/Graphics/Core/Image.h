#pragma once

/**
*	@file Image.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 08/04/2019
*/

/**
*	@brief Image wrapper for a set of bits with associated information.
*/
class Image
{
protected:
	std::vector<unsigned char> _image;					//!< Image bits
	unsigned _width, _height, _depth;					//!< Image dimensions

protected:
	/**
	*	@brief Writes an image in file system in an isolated thread, so the application doesn't get stuck.
	*	@param pixels Pixels of image to be written.
	*	@param filename Path where the image must be written.
	*	@param width Width of image.
	*	@param height Height of image.
	*/
	void threadedWriteImage(std::vector<GLubyte>* pixels, const std::string& filename, const uint16_t width, const uint16_t height);

public:
	/**
	*	@brief Constructor of image from a filename from the system.
	*	@param filename Path of image.
	*/
	Image(const std::string& filename);

	/**
	*	@brief Constructor of already loaded image, so we just acts as a wrapper.
	*	@param image Array of image bits.
	*	@param width Image width.
	*	@param height Image height.
	*	@param depth Number of channels.
	*/
	Image(unsigned char* image, const uint16_t width, const uint16_t height, const uint8_t depth);

	/**
	*	@brief Destructor.
	*/
	~Image();

	/**
	*	@return Array of bits from image.
	*/
	unsigned char* bits() { return _image.data(); }

	/**
	*	@brief Flips the image so that bottom side turns into the upper side.
	*	@param image Array of image bits.
	*	@param width Image width.
	*	@param height Image height.
	*	@param depth Number of channels.
	*/
	void flipImageVertically();

	/**
	*	@brief Saves the captured scene in a file.
	*	@param filename Path of file.
	*/
	void saveImage(const std::string& filename);
	
	// ---------- Getters ------------

	/**
	*	@return Number of channels of image.
	*/
	int getDepth() const { return _depth; }

	/**
	*	@return Height of image.
	*/
	int getHeight() const { return _height; }

	/**
	*	@return Width of image.
	*/
	int getWidth() const { return _width; }

	// ----------- Static methods ------------

	/**
	*	@brief Flips the image so that bottom side turns into the upper side.
	*	@param image Array of image bits.
	*	@param width Image width.
	*	@param height Image height.
	*	@param depth Number of channels.
	*/
	static void flipImageVertically(std::vector<unsigned char>& image, const uint16_t width, const uint16_t height, const uint8_t depth);
};

