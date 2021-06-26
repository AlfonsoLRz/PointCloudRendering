#pragma once

/**
*	@file ShaderProgram.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/10/2019
*/

/**
*	@brief Shader wrapper, including the several pipelines shaders to draw any scene.
*		   !!!! Compute shaders are always isolated.
*/
class ShaderProgram
{
public:
	enum ShaderTypes: uint8_t
	{
		VERTEX_SHADER, 
		FRAGMENT_SHADER,
		GEOMETRY_SHADER, 
		TESSELATION_CONTROL_SHADER,
		TESSELATION_EVAL_SHADER,
		COMPUTE_SHADER
	};

protected:
	const static std::string MODULE_HEADER;
	const static std::string MODULE_FILE_CHAR_1;
	const static std::string MODULE_FILE_CHAR_2;

protected:
	GLuint				_handler;												//!< Shader program id in GPU
	bool				_linked;												//!< Flag which tell us if the shader has been linked correctly
	std::string			_logString;												//!< Error message got from the last operation with the shader

	// [Subroutines]
	std::vector<GLuint> _activeSubroutineUniform[COMPUTE_SHADER + 1];			//!< Active uniform for each subroutine for each shader type

	// [Libraries]
	static std::unordered_map<std::string, std::string> _moduleCode;			//!< Modules that are already loaded

protected:
	/**
	*	@brief Tries to compile a shader at a certain path.
	*	@param filename Path of the shader we need to compile.
	*	@param shaderType Type of shader to be compiled: VERTEX, FRAGMENT, GEOMETRY or COMPUTE.
	*	@return Assigned ID for the shader.
	*/
	virtual GLuint compileShader(const char* filename, const GLenum shaderType);

	/**
	*	@brief Checks if the file exists in the system.
	*	@param filename Path of file in the system.
	*/
	bool fileExists(const std::string& fileName);

	/**
	*	@brief Transforms an OpenGL shader value to our own values from the enum ShaderTypes.
	*	@param shaderType OpenGL shader enum.
	*/
	ShaderTypes fromOpenGLToShaderTypes(const GLenum shaderType);

	/**
	*	@brief Substitutes the libraries references by its code.
	*	@param shaderContent Shader code to be modified.
	*/
	bool includeLibraries(std::string& shaderContent);

	/**
	*	@brief Reads the content of a shader.
	*	@param filename Path in the system where the shader is located.
	*	@param content Code obtained from the shader file.
	*/
	virtual bool loadFileContent(const std::string& filename, std::string& content);

public:
	/**
	*	@brief Constructor.
	*/
	ShaderProgram();

	/**
	*	@brief Destructor.
	*/
	~ShaderProgram();

	/**
	*	@brief Applies all the active subroutines.
	*/
	virtual void applyActiveSubroutines() = 0;

	/**
	*	@brief Reads and compiles all the shaders which contain the name specified as an argument.
	*	@filename filename Shader program name.
	*/
	virtual GLuint createShaderProgram(const char* filename) = 0;

	/**
	*	@brief Modifies the active uniform on a subroutine.
	*	@param shaderType Shader type where we desire to modify the subroutine uniform.
	*	@param subroutine Name of subroutine to be modified in a shader.
	*	@param functionName Name of active uniform in the specified subroutine.
	*/
	bool setSubroutineUniform(const GLenum shaderType, const std::string& subroutine, const std::string& functionName);

	/**
	*	@brief Modifies the value of a floating value uniform.
	*	@param name Name of uniform.
	*	@param value Value of uniform.
	*/
	bool setUniform(const std::string& name, const GLfloat value);

	/**
	*	@brief Modifies the value of an integer uniform.
	*	@param name Name of uniform.
	*	@param value Value of uniform.
	*/
	bool setUniform(const std::string& name, const GLint value);

	/**
	*	@brief Modifies the value of an unsigned integer uniform.
	*	@param name Name of uniform.
	*	@param value Value of uniform.
	*/
	bool setUniform(const std::string& name, const GLuint value);

	/**
	*	@brief Modifies the value of a 4x4 matrix uniform.
	*	@param name Name of uniform.
	*	@param value Value of uniform.
	*/
	bool setUniform(const std::string& name, const mat4& value);

	/**
	*	@brief Modifies the value of a Vector2 uniform.
	*	@param name Name of uniform.
	*	@param value Value of uniform.
	*/
	bool setUniform(const std::string& name, const vec2& value);

	/**
	*	@brief Modifies the value of an unsigned Vector2 uniform.
	*	@param name Name of uniform.
	*	@param value Value of uniform.
	*/
	bool setUniform(const std::string& name, const uvec2& value);

	/**
	*	@brief Modifies the value of a Vector3 uniform.
	*	@param name Name of uniform.
	*	@param value Value of uniform.
	*/
	bool setUniform(const std::string& name, const vec3& value);

	/**
	*	@brief Modifies the value of a Vector4 uniform.
	*	@param name Name of uniform.
	*	@param value Value of uniform.
	*/
	bool setUniform(const std::string& name, const vec4& value);

	/**
	*	@brief Actives this shader program and so the following drawing lines are affected and drawn by this code.
	*/
	bool use();
};

