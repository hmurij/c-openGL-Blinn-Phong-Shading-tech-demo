#pragma once

#include <fstream>
#include <sstream>
#include <string>

// Include GLM core features
#include <glm/vec2.hpp>           // vec2
#include <glm/vec3.hpp>           // vec3
#include <glm/mat4x4.hpp>         // mat4

enum ShaderType { NONE = -1, VERTEX = 0, TESS_CONTROL = 1, TESS_EVALUATION = 2, GEOMETRY = 3, FRAGMENT = 4, PROGRAM = 5 };

class ShaderProgram
{
public:
	ShaderProgram(const char* shader_source_file_path) {

		program_ID = glCreateProgram();

		// set up and compile vertex shader
		compileAttachDeleteShader(readShaderFromFile(shader_source_file_path, ShaderType::VERTEX), ShaderType::VERTEX);

		// set up and compile tessellation control shader
		// compileAttachDeleteShader(readShaderFromFile(shader_source_file_path, ShaderType::TESS_CONTROL), ShaderType::TESS_CONTROL);

		// set up and compile tessellation evaluation shader
		// compileAttachDeleteShader(readShaderFromFile(shader_source_file_path, ShaderType::TESS_EVALUATION), ShaderType::TESS_EVALUATION);

		// set up and compile geometry shader
		//compileAttachDeleteShader(readShaderFromFile(shader_source_file_path, ShaderType::GEOMETRY), ShaderType::GEOMETRY);

		// set up and compile fragemnt shader
		compileAttachDeleteShader(readShaderFromFile(shader_source_file_path, ShaderType::FRAGMENT), ShaderType::FRAGMENT);

		glLinkProgram(program_ID);
		checkCompileErrors(program_ID, ShaderType::PROGRAM);

	} // end of constructor

	void use() {
		glUseProgram(program_ID);
	} // end of use

	void link() {
		glLinkProgram(program_ID);
	}

	  // utility uniform functions
	  // ------------------------------------------------------------------------
	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(program_ID, name.c_str()), (int)value);
	}
	// ------------------------------------------------------------------------
	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(program_ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(program_ID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void setVec2(const std::string &name, const glm::vec2 &value) const
	{
		glUniform2fv(glGetUniformLocation(program_ID, name.c_str()), 1, &value[0]);
	}
	void setVec2(const std::string &name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(program_ID, name.c_str()), x, y);
	}
	// ------------------------------------------------------------------------
	void setVec3(const std::string &name, const glm::vec3 &value) const
	{
		glUniform3fv(glGetUniformLocation(program_ID, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string &name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(program_ID, name.c_str()), x, y, z);
	}
	// ------------------------------------------------------------------------
	void setVec4(const std::string &name, const glm::vec4 &value) const
	{
		glUniform4fv(glGetUniformLocation(program_ID, name.c_str()), 1, &value[0]);
	}
	void setVec4(const std::string &name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(program_ID, name.c_str()), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	void setMat2(const std::string &name, const glm::mat2 &mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(program_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat3(const std::string &name, const glm::mat3 &mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(program_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void setMat4(const std::string &name, const glm::mat4 &mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(program_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	inline unsigned int get_program_ID() { return program_ID; };

private:
	unsigned int program_ID;

	std::string readShaderFromFile(const char * shader_source_file_path, ShaderType shader_type) {

		std::ifstream source_file(shader_source_file_path);
		std::stringstream shader_source_code_buffer;

		if (source_file.is_open()) {

			std::cout << "Opened file path: " << shader_source_file_path << std::endl
				<< "-- --------------------------------------------------------------------------------------------- -- " << std::endl;

			ShaderType current_shader_type = ShaderType::NONE;

			std::string line;


			while (std::getline(source_file, line)) {

				if (line.find("#vertex shader") != std::string::npos) {
					// std::cout << "set shader type to vertex" << std::endl;
					current_shader_type = ShaderType::VERTEX;
				}
				else if (line.find("#tessellation control shader") != std::string::npos) {
					// std::cout << "set shader type to fragment" << std::endl;
					current_shader_type = ShaderType::TESS_CONTROL;
				}
				else if (line.find("#tessellation evaluation shader") != std::string::npos) {
					// std::cout << "set shader type to fragment" << std::endl;
					current_shader_type = ShaderType::TESS_EVALUATION;
				}
				else if (line.find("#geometry shader") != std::string::npos) {
					// std::cout << "set shader type to fragment" << std::endl;
					current_shader_type = ShaderType::GEOMETRY;
				}
				else if (line.find("#fragment shader") != std::string::npos) {
					// std::cout << "set shader type to fragment" << std::endl;
					current_shader_type = ShaderType::FRAGMENT;
				}
				else if (current_shader_type == shader_type) {
					shader_source_code_buffer << line << '\n';
				}

			} // end of while

			std::string shaders_source_code_string = shader_source_code_buffer.str();

			source_file.close();

		} // end of if
		else {
			std::cout << "Can't open file path: " << shader_source_file_path << std::endl
				<< "-- --------------------------------------------------------------------------------------------- -- " << std::endl;
		} // end of else

		return shader_source_code_buffer.str();

	} // end of readShaderFromFile

	std::string getShaderString(ShaderType shader_type) {
		std::string shader_type_str;
		switch (shader_type) {
		case ShaderType::VERTEX:
			shader_type_str = "VERTEX";
			break;
		case ShaderType::TESS_CONTROL:
			shader_type_str = "TESSELLATION CONTROL";
			break;
		case ShaderType::TESS_EVALUATION:
			shader_type_str = "TESSELLATION EVALUATION";
			break;
		case ShaderType::GEOMETRY:
			shader_type_str = "GEOMETRY";
			break;
		case ShaderType::FRAGMENT:
			shader_type_str = "FRAGMENT";
			break;

		} // end of switch

		return shader_type_str;
	} // end of getShaderString

	GLenum getShaderType(ShaderType shader_type) {
		GLenum shader_type_GLenum;

		switch (shader_type) {
		case ShaderType::VERTEX:
			shader_type_GLenum = GL_VERTEX_SHADER;
			break;
		case ShaderType::TESS_CONTROL:
			shader_type_GLenum = GL_TESS_CONTROL_SHADER;
			break;
		case ShaderType::TESS_EVALUATION:
			shader_type_GLenum = GL_TESS_EVALUATION_SHADER;
			break;
		case ShaderType::GEOMETRY:
			shader_type_GLenum = GL_GEOMETRY_SHADER;
			break;
		case ShaderType::FRAGMENT:
			shader_type_GLenum = GL_FRAGMENT_SHADER;
			break;

		} // end of switch

		return shader_type_GLenum;
	} // end of getShaderType

	void compileAttachDeleteShader(std::string shader_code_string, ShaderType shader_type) {
		if (shader_code_string.length() != 0) {
			const char * shader_source_code = shader_code_string.c_str();
			unsigned int shader;
			shader = glCreateShader(getShaderType(shader_type));
			glShaderSource(shader, 1, &shader_source_code, NULL);
			std::cout << getShaderString(shader_type) << " shader source code" << std::endl
				<< "-- --------------------------------------------------------------------------------------------- -- " << std::endl
				<< shader_source_code << std::endl;
			glCompileShader(shader);
			checkCompileErrors(shader, shader_type);
			glAttachShader(program_ID, shader);
			glDeleteShader(shader);
		}
	} // end of compileAttachDeleteShader

	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	void checkCompileErrors(unsigned int shader, ShaderType shader_type)
	{
		int success;
		char infoLog[1024];
		if (shader_type != ShaderType::PROGRAM)
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "-- --------------------------------------------------------------------------------------------- -- " << std::endl
					<< "ERROR::SHADER_COMPILATION_ERROR of type: " << getShaderString(shader_type)
					<< "\n" << infoLog << "\n-- --------------------------------------------------------------------------------------------- -- " << std::endl;
			}
			else {
				std::cout << "-- --------------------------------------------------------------------------------------------- -- " << std::endl
					<< "COMPILE STATUS - NO ERRORS"
					<< "\n-- --------------------------------------------------------------------------------------------- -- " << std::endl;
			}

		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "\n-- --------------------------------------------------------------------------------------------- -- \n"
					<< "ERROR::PROGRAM_LINKING_ERROR of type: " << "PROGRAM"
					<< "\n" << infoLog << "\n-- --------------------------------------------------------------------------------------------- -- " << std::endl;
			}
			else {
				std::cout << "-- --------------------------------------------------------------------------------------------- -- " << std::endl
					<< "SHADER PROGRAM COMPILE STATUS - NO ERRORS"
					<< "\n-- --------------------------------------------------------------------------------------------- -- " << std::endl;
			}
		}
	}

};

