#include "stdafx.h"
#include "ShaderProgram.h"

// [Static variables initialization]

const std::string ShaderProgram::MODULE_HEADER = "#include";
const std::string ShaderProgram::MODULE_FILE_CHAR_1 = "<";
const std::string ShaderProgram::MODULE_FILE_CHAR_2 = ">";

std::unordered_map<std::string, std::string> ShaderProgram::_moduleCode;

/// [Public methods]

ShaderProgram::ShaderProgram()
	: _handler(0), _linked(false), _logString("")
{
}

ShaderProgram::~ShaderProgram()
{
}

bool ShaderProgram::setSubroutineUniform(const GLenum shaderType, const std::string& subroutine, const std::string& functionName)
{
	GLint subroutineID	= glGetSubroutineUniformLocation(_handler, shaderType, subroutine.c_str());
	GLint uniformID		= glGetSubroutineIndex(_handler, shaderType, functionName.c_str());

	if (subroutineID >= 0 && uniformID >= 0)			// OpenGL returns -1 if subroutine or uniform doesn't exist
	{
		ShaderTypes shaderEnum = fromOpenGLToShaderTypes(shaderType);
		_activeSubroutineUniform[shaderEnum][subroutineID] = uniformID;

		return true;
	}

	return false;
}

bool ShaderProgram::setUniform(const std::string& name, GLfloat value)
{
	GLint location = glGetUniformLocation(_handler, name.c_str());

	if (location >= 0)
	{
		glUniform1f(location, value);

		return true;
	}

	std::cout << "Cannot find localization for: " << name << std::endl;
	
	return false;
}

bool ShaderProgram::setUniform(const std::string& name, GLint value)
{
	GLint location = glGetUniformLocation(_handler, name.c_str());

	if (location >= 0)
	{
		glUniform1i(location, value);

		return true;
	}
	
	std::cout << "Cannot find localization for: " << name << std::endl;

	return false;
}

bool ShaderProgram::setUniform(const std::string& name, const GLuint value)
{
	GLint location = glGetUniformLocation(_handler, name.c_str());

	if (location >= 0)
	{
		glUniform1ui(location, value);

		return true;
	}

	std::cout << "Cannot find localization for: " << name << std::endl;

	return false;
}

bool ShaderProgram::setUniform(const std::string& name, const mat4& value)
{
	GLint location = glGetUniformLocation(_handler, name.c_str());

	if (location >= 0)
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);

		return true;
	}

	std::cout << "Cannot find localization for: " << name << std::endl;

	return false;
}

bool ShaderProgram::setUniform(const std::string& name, const vec2& value)
{
	GLint location = glGetUniformLocation(_handler, name.c_str());

	if (location >= 0)
	{
		glUniform2fv(location, 1, &value[0]);

		return true;
	}
	
	std::cout << "Cannot find localization for: " << name << std::endl;

	return false;
}

bool ShaderProgram::setUniform(const std::string& name, const uvec2& value)
{
	GLint location = glGetUniformLocation(_handler, name.c_str());

	if (location >= 0)
	{
		glUniform2uiv(location, 1, &value[0]);

		return true;
	}

	std::cout << "Cannot find localization for: " << name << std::endl;

	return false;
}

bool ShaderProgram::setUniform(const std::string& name, const vec3& value)
{
	GLint location = glGetUniformLocation(_handler, name.c_str());

	if (location >= 0)
	{
		glUniform3fv(location, 1, &value[0]);

		return true;
	}

	std::cout << "Cannot find localization for: " << name << std::endl;

	return false;
}

bool ShaderProgram::setUniform(const std::string& name, const vec4& value)
{
	GLint location = glGetUniformLocation(_handler, name.c_str());

	if (location >= 0)
	{
		glUniform4fv(location, 1, &value[0]);

		return true;
	}

	std::cout << "Cannot find localization for: " << name << std::endl;

	return false;
}

bool ShaderProgram::use()
{
	if ((_handler > 0) && (_linked))			// Is the program created and linked?
	{
		glUseProgram(_handler);

		return true;
	}

	std::cout << "Cannot use shader program" << std::endl;

	return false;
}

/// [Protected methods]

GLuint ShaderProgram::compileShader(const char* filename, const GLenum shaderType)
{
	if (!fileExists(filename))
	{
		if (shaderType != GL_GEOMETRY_SHADER)
			fprintf(stderr, "Shader source file %s not found.\n", filename);

		return 0;
	}

	std::string shaderSourceString;
	if (!loadFileContent(std::string(filename), shaderSourceString))						// Read shader code
	{
		fprintf(stderr, "Cannot open shader source file.\n");
		return 0;
	}

	if (!includeLibraries(shaderSourceString))												// Libraries code not found 
	{
		fprintf(stderr, "Cannot include the specified modules.\n");
		return 0;
	}

	GLuint shaderHandler = glCreateShader(shaderType);
	if (shaderHandler == 0)
	{
		fprintf(stderr, "Cannot create shader object.\n");
		return 0;
	}

	const char* shaderSourceCString = shaderSourceString.c_str();							// Compile shader code
	glShaderSource(shaderHandler, 1, &shaderSourceCString, NULL);
	glCompileShader(shaderHandler);

	GLint compileResult;
	glGetShaderiv(shaderHandler, GL_COMPILE_STATUS, &compileResult);					// Result

	if (compileResult == GL_FALSE)
	{
		GLint logLen = 0;
		_logString = "";
		glGetShaderiv(shaderHandler, GL_INFO_LOG_LENGTH, &logLen);

		if (logLen > 0)
		{
			char* cLogString = new char[logLen];
			GLint written = 0;

			glGetShaderInfoLog(shaderHandler, logLen, &written, cLogString);
			_logString.assign(cLogString);

			delete[] cLogString;
			std::cout << "Cannot compile shader " << shaderType << std::endl << _logString << std::endl;
		}
	}

	return shaderHandler;
}

bool ShaderProgram::fileExists(const std::string& fileName)
{
	struct stat info; 
	int ret = -1;

	ret = stat(fileName.c_str(), &info);

	return 0 == ret;
}

ShaderProgram::ShaderTypes ShaderProgram::fromOpenGLToShaderTypes(const GLenum shaderType)
{
	switch (shaderType)
	{
	case GL_VERTEX_SHADER: return VERTEX_SHADER;
	case GL_FRAGMENT_SHADER: return FRAGMENT_SHADER;
	case GL_GEOMETRY_SHADER: return GEOMETRY_SHADER;
	case GL_COMPUTE_SHADER: return COMPUTE_SHADER;
	case GL_TESS_CONTROL_SHADER: return TESSELATION_CONTROL_SHADER;
	case GL_TESS_EVALUATION_SHADER: return TESSELATION_EVAL_SHADER;
	}

	return VERTEX_SHADER;
}

bool ShaderProgram::includeLibraries(std::string& shaderContent)
{
	size_t pos = shaderContent.find(MODULE_HEADER);

	while (pos != std::string::npos)
	{
		const std::size_t char_1 = shaderContent.find(MODULE_FILE_CHAR_1, pos);
		const std::size_t char_2 = shaderContent.find(MODULE_FILE_CHAR_2, pos);

		if ((char_1 == std::string::npos) || (char_2 == std::string::npos))				// Incorrect syntax
		{
			return false;
		}

		const std::string module = shaderContent.substr(char_1 + 1, char_2 - char_1 - 1);
		if (!fileExists(module))														// Library refers to a new file, does it exist?
		{
			return false;
		}

		const auto moduleCode = _moduleCode.find(module);								// If file is already read we can just retrieve the string
		std::string moduleCodeStr;
		if (moduleCode == _moduleCode.end())
		{
			if (!loadFileContent(module, moduleCodeStr))
			{
				return false;
			}

			_moduleCode[module] = moduleCodeStr;
		}
		else
		{
			moduleCodeStr = moduleCode->second;
		}
				
		shaderContent.erase(shaderContent.begin() + pos, shaderContent.begin() + char_2 + 1);			// Replace string in shader code
		shaderContent.insert(pos, moduleCodeStr.c_str());
		pos = shaderContent.find(MODULE_HEADER);
	}

	return true;
}

bool ShaderProgram::loadFileContent(const std::string& filename, std::string& content)
{
	std::ifstream shaderSourceFile;
	shaderSourceFile.open(filename);

	if (!shaderSourceFile)
	{
		return false;
	}

	std::stringstream shaderSourceStream;
	shaderSourceStream << shaderSourceFile.rdbuf();
	content = shaderSourceStream.str();
	shaderSourceFile.close();

	return true;
}
