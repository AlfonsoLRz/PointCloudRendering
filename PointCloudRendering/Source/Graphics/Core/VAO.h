#pragma once

#include "Graphics/Core/GraphicsCoreEnumerations.h"
#include "Graphics/Core/Model3D.h"

/**
*	@file VAO.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/10/2019
*/

/**
*	@brief Structure which wraps the geometry and topology of any object.
*/
class VAO
{
protected:
	// [VAO, VBO and IBO identifiers]
	GLuint				_vao;								//!< Identifier of wrapper of VBOs and IBOs
	std::vector<GLuint> _vbo;								//!< One per defined geometry
	std::vector<GLuint> _ibo;								//!< One per defined topology

	int					_vboIndex;							//!< Controls index of next VBO

protected:
	/**
	*	@brief Defines an VBO just for one type of geometry.
	*	@param vboType Geometry type.
	*	@param dataSize Size of each data value. Example: sizeof(vec3) if vec3 is the geometry data at each index.
	*	@param unitType Primitive of data: GL_FLOAT, etc.
	*	@param shaderSlot Input channel of VBO.
	*/
	void declareSimpleVBO(const RendEnum::VBOTypes vboType, const GLsizei dataSize, const GLuint unitType, int& shaderSlot);

	/**
	*	@brief Defines an VBO just for data which combines several types of geometry.
	*	@param vboType Geometry type.
	*	@param totalSize Size of complete data.
	*	@param dataSize Size of each data value for each geometry.
	*	@param unitType Primitive of data: GL_FLOAT, etc.
	*	@param shaderSlot Input channel of VBO.
	*/
	void declareInterleavedVBO(const RendEnum::VBOTypes vboType, const GLsizei totalSize, const std::vector<unsigned long long>& dataSize, const std::vector<GLuint>& unitType, int& shaderSlot);

	/**
	*	@brief Defines the geometry structure for VertexGPUData buffer.
	*/
	void genGPUGeometryVBO();

public:
	/**
	*	@brief Default constructor. Does not book any space in GPU for any geometry / topology.
	*/
	VAO(bool gpuGeometry = false);

	/**
	*	@brief Unsupported copy constructor.
	*/
	VAO(const VAO& orig) = delete;

	/**
	*	@brief Destructor.
	*/
	virtual ~VAO();

	/**
	*	@brief Unsupported assignment operator.
	*/
	VAO& operator=(const VAO& orig) = delete;

	/**
	*	@brief Draws an object with an specific topology.
	*	@param type IBO que debemos utilizar para dibujar el objeto.
	*	@param openGLPrimitive Primitiva que usaremos al dibujar: GL_POINTS, GL_TRIANGLE_STRIP...
	*	@param numIndices Número de índices que se considerarán al dibujar.
	*/
	void drawObject(const RendEnum::IBOTypes iboType, const GLuint openGLPrimitive, const GLuint numIndices);

	/**
	*	@brief Draws an object with an specific topology.
	*	@param type IBO que debemos utilizar para dibujar el objeto.
	*	@param openGLPrimitive Primitiva que usaremos al dibujar: GL_POINTS, GL_TRIANGLE_STRIP...
	*	@param numIndices Número de índices que se considerarán al dibujar.
	*	@param numObjects Number of repetitions of draw call.
	*/
	void drawObject(const RendEnum::IBOTypes iboType, const GLuint openGLPrimitive, const GLuint numIndices, const GLuint numObjects);

	/**
	*	@brief Draws an object with an specific topology.
	*	@param type IBO que debemos utilizar para dibujar el objeto.
	*	@param openGLPrimitive Primitiva que usaremos al dibujar: GL_POINTS, GL_TRIANGLE_STRIP...
	*	@param numIndices Número de índices que se considerarán al dibujar.
	*	@param numObjects Number of repetitions of draw call.
	*/
	void drawObject(const GLuint openGLPrimitive, const GLuint numIndices, const GLuint numObjects);

	/**
	*	@brief Sets data in the VBO.
	*	@param vboType VBO to be modified.
	*	@param geometryData Geometry data to be put on the vbo space.
	*/
	template<typename T>
	void setVBOData(const RendEnum::VBOTypes vboType, const std::vector<T>& geometryData, const GLuint changeFrequency = GL_STATIC_DRAW);

	/**
	*	@brief Sets data in the VBO.
	*	@param vboType VBO to be modified.
	*	@param geometryData Geometry data to be put on the vbo space.
	*/
	template<typename T>
	void setVBOData(const RendEnum::VBOTypes vboType, T* geometryData, const GLuint size, const GLuint changeFrequency = GL_STATIC_DRAW);

	/**
	*	@brief Sets the VertexGPUData in VBO.
	*/
	void setVBOData(const std::vector<Model3D::VertexGPUData>& geometryData, const GLuint changeFrequency = GL_STATIC_DRAW);

	/**
	*	@brief Sets indices in IBO.
	*	@param iboType IBO to be modified.
	*	@param geometryData Data to be put on the ibo space.
	*/
	void setIBOData(const RendEnum::IBOTypes iboType, const std::vector<GLuint>& topologyData, const GLuint changeFrequency = GL_STATIC_DRAW);
};

template<typename T>
void VAO::setVBOData(const RendEnum::VBOTypes vboType, const std::vector<T>& geometryData, const GLuint changeFrequency)
{
	glBindVertexArray(_vao);

	{
		glBindBuffer(GL_ARRAY_BUFFER, _vbo[vboType]);

		/**
		*	First argument: GL_ARRAY_BUFFER since its an VBO
		*   Second argument: number of bytes (length of geometry)
		*	Third argument: low level geometry array
		*	Fourth argument: change frecuency of the specified geometry
		*/
		glBufferData(GL_ARRAY_BUFFER, geometryData.size() * sizeof(T), geometryData.data(), changeFrequency);
	}
}

template<typename T>
void VAO::setVBOData(const RendEnum::VBOTypes vboType, T* geometryData, const GLuint size, const GLuint changeFrequency)
{
	glBindVertexArray(_vao);

	{
		glBindBuffer(GL_ARRAY_BUFFER, _vbo[vboType]);

		/**
		*	First argument: GL_ARRAY_BUFFER since its an VBO
		*   Second argument: number of bytes (length of geometry)
		*	Third argument: low level geometry array
		*	Fourth argument: change frecuency of the specified geometry
		*/
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(T), geometryData, changeFrequency);
	}
}
