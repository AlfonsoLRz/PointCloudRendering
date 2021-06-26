#pragma once

#include "Graphics/Core/ShaderProgram.h"

/**
*	@file ComputeShader.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/13/2019
*/

/**
*	@brief Shader for parallel computing.
*/
class ComputeShader: public ShaderProgram
{
public:
	enum WorkGroupAxis { X_AXIS, Y_AXIS, Z_AXIS };

protected:
	static std::vector<GLint> MAX_WORK_GROUP_SIZE;					//!< This value can be useful since the number of groups is not as limited as group size

public:	
	/**
	*	@brief Default constructor.
	*/
	ComputeShader();

	/**
	*	@brief Destructor.
	*/
	virtual ~ComputeShader();

	/**
	*	@brief Applies all the active subroutines.
	*/
	virtual void applyActiveSubroutines();

	/**
	*	@brief Binds all those buffers which have been defined (either in / out buffers).
	*/
	void bindBuffers(const std::vector<GLuint>& bufferID);

	/**
	*	@brief Reads and compiles the compute shader located at filename, which is followed by string -comp.glsl.
	*	@filename filename Shader program name. Example: pointCloud => parallel-comp.glsl.
	*/
	virtual GLuint createShaderProgram(const char* filename);

	/**
	*	@brief Executes the compute shader with many groups and works as specified and waits till the execution is over.
	*/
	void execute(GLuint numGroups_x, GLuint numGroups_y, GLuint numGroups_z, GLuint workGroup_x, GLuint workGroup_y, GLuint workGroup_z);

	/**
	*	@return Maximum size a work group can get.
	*/
	static std::vector<GLint> getMaxLocalSize();

	/**
	*	@return Maximum number of threads per group in any axis.
	*/
	static GLint getMaxGroupSize(const WorkGroupAxis axis = X_AXIS) { return MAX_WORK_GROUP_SIZE[axis]; }

	/**
	*	@return Number of groups which are neccesary to cover the array size taking into account the maximum size of a work group.
	*/
	static int getNumGroups(const unsigned arraySize, const WorkGroupAxis axis = X_AXIS);

	/**
	*	@return Size of each group taking into account the total number of elements.
	*/
	static int getWorkGroupSize(const unsigned numGroups, const unsigned arraySize);

	/**
	*	@brief Retrieves maximum group size once OpenGL context is ready.
	*/
	static void initializeMaxGroupSize();

	/**
	*	@brief Retrieves data from GPU.
	*	@param bufferID Identifier of buffer in GPU.
	*	@param Type of data to be retrieved. Any value can be used here.
	*/
	template<typename T>
	static T* readData(GLuint bufferID, const T& dataType);

	/**
	*	@brief Sets a new uniform which correspondons to an input image.
	*/
	void setImageUniform(const GLint id, const std::string& shaderVariable);

	/**
	*	@brief Defines a new buffer at OpenGL context.
	*	@return ID of new buffer, so we can ask for reading operations.
	*/
	template<typename T>
	static GLuint setReadBuffer(const std::vector<T>& data, const GLuint changeFrequency = GL_DYNAMIC_DRAW);

	/**
	*	@brief Defines a new buffer at OpenGL context.
	*	@return ID of new buffer, so we can ask for reading operations.
	*/
	template<typename T>
	static GLuint setReadBuffer(const T* data, const unsigned arraySize, const GLuint changeFrequency = GL_DYNAMIC_DRAW);

	/**
	*	@brief Defines a new buffer at OpenGL context.
	*	@return ID of new buffer, so we can ask for reading operations.
	*/
	template<typename T>
	static GLuint setReadData(const T& data, const GLuint changeFrequency = GL_DYNAMIC_DRAW);

	/**
	*	@brief Defines a new writing buffer at OpenGL context.
	*	@return ID of new buffer, so we can ask for reading operations.
	*/
	template<typename T>
	static GLuint setWriteBuffer(const T& dataType, const GLuint arraySize, const GLuint changeFrequency = GL_DYNAMIC_DRAW);

	/**
	*	@brief Defines a new buffer at OpenGL context.
	*/
	template<typename T>
	static void updateReadBuffer(const GLuint id, const T* data, const unsigned arraySize, const GLuint changeFrequency = GL_DYNAMIC_DRAW);

	/**
	*	@brief Updates a previously defined writing buffer.
	*/
	template<typename T>
	static void updateWriteBuffer(const GLuint id, const T& dataType, const unsigned arraySize, const GLuint changeFrequency = GL_DYNAMIC_DRAW);
};

template<typename T>
inline T* ComputeShader::readData(GLuint bufferID, const T& dataType)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);																	
	T* data = (T*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER); 

	return data;
}

template<typename T>
inline GLuint ComputeShader::setReadBuffer(const std::vector<T>& data, const GLuint changeFrequency)
{
	GLuint id;
	glGenBuffers(1, &id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T) * data.size(), data.data(), changeFrequency);

	return id;
}

template<typename T>
inline GLuint ComputeShader::setReadBuffer(const T* data, const unsigned arraySize, const GLuint changeFrequency)
{
	GLuint id;
	glGenBuffers(1, &id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T) * arraySize, data, changeFrequency);

	return id;
}

template<typename T>
inline GLuint ComputeShader::setReadData(const T& data, const GLuint changeFrequency)
{
	GLuint id;
	glGenBuffers(1, &id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T), &data, changeFrequency);

	return id;
}

template<typename T>
inline GLuint ComputeShader::setWriteBuffer(const T& dataType, const GLuint arraySize, const GLuint changeFrequency)
{
	GLuint id;
	glGenBuffers(1, &id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(dataType) * arraySize, nullptr, changeFrequency);

	return id;
}

template<typename T>
inline void ComputeShader::updateReadBuffer(const GLuint id, const T* data, const unsigned arraySize, const GLuint changeFrequency)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T) * arraySize, data, changeFrequency);
}

template<typename T>
inline void ComputeShader::updateWriteBuffer(const GLuint id, const T& dataType, const unsigned arraySize, const GLuint changeFrequency)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(dataType) * arraySize, nullptr, changeFrequency);
}
