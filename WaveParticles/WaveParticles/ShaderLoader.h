#pragma once

// STANDARD
#include <string>
#include <string.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <map>

// CUSTOM
#include "OpenGL.h"
#include "ShaderType.h"
#include "FileIO.h"


namespace Core::Shaders
{
	// load, compile, and return a shader of the specified `type`
	GLuint loadShader(GLenum type, const char* path)
	{
		// load shader file
		std::string shader_str = Core::FileIO::readFileContents(path);
		const char* shader_src = shader_str.c_str();

		// create shader
		GLuint shader = glCreateShader(type);
		std::string shader_type;

		// Compile vertex shader
		switch (type)
		{
		case GL_VERTEX_SHADER:
			shader_type = "vertex";
			break;
		case GL_GEOMETRY_SHADER:
			shader_type = "geometry";
			break;
		case GL_FRAGMENT_SHADER:
			shader_type = "fragment";
			break;

			/*
		case GL_COMPUTE_SHADER:
			shader_type = "compute";
			std::cerr << "Error: Unsupported shader type ("
				<< shader_type << " shader)"
				<< std::endl;
			break;
		case GL_TESS_CONTROL_SHADER:
			shader_type = "tesselation control";
			std::cerr << "Error: Unsupported shader type ("
				<< shader_type << " shader)"
				<< std::endl;
			break;
		case GL_TESS_EVALUATION_SHADER:
			shader_type = "tesselation evaluation";
			std::cerr << "Error: Unsupported shader type ("
				<< shader_type << " shader)"
				<< std::endl;
			break;
			*/

		default:
			std::cerr << "Error: Unrecognized shader type" << std::endl;
			return 0;
		}
		glShaderSource(shader, 1, &shader_src, NULL);
		glCompileShader(shader);

		// check if compilation was successful
		GLint result = GL_FALSE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

		// if compilation did not succeed, print the error message
		if (!result)
		{
			fprintf(stderr, "---> ERROR: compiling %s shader!\n", shader_type.c_str());

			int logLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
			std::vector<GLchar> shader_err((logLength > 1) ? logLength : 1);
			glGetShaderInfoLog(shader, logLength, NULL, &shader_err[0]);

			fprintf(stderr, "%s\n", &shader_err[0]);
		}

		return shader;
	}

	const std::string SHADER_FILE_EXTENSION = ".shd";


	GLuint LoadVertexShader(const std::string& shaderDir)
	{
		std::string filePath = shaderDir + "vertex" + SHADER_FILE_EXTENSION;
		return loadShader(GL_VERTEX_SHADER, filePath.c_str());
	}

	GLuint LoadGeometryShader(const std::string& shaderDir)
	{
		std::string filePath = shaderDir + "geometry" + SHADER_FILE_EXTENSION;
		return loadShader(GL_GEOMETRY_SHADER, filePath.c_str());
	}

	GLuint LoadFragmentShader(const std::string& shaderDir)
	{
		std::string filePath = shaderDir + "fragment" + SHADER_FILE_EXTENSION;
		return loadShader(GL_FRAGMENT_SHADER, filePath.c_str());
	}


	GLuint LoadTransformFeedbackShaderProgram(const char* path,
		TransformFeedbackShaderType type, const char** outputs, int numOutputs)
	{
		std::vector<GLuint> shaders;
		const std::string shaderDir = Core::FileIO::getPlatformPath(path);

		std::cout << "Testing TF shader outputs:" << std::endl;
		for (int i = 0; i < numOutputs; i++)
		{
			std::cout << outputs[i] << std::endl;
		}
		std::cout << std::endl;

		switch (type)
		{
		case TF_SHADER_TYPE_V:
			printf("Loading V shader program '%s'\n", shaderDir.c_str());
			shaders.push_back(LoadVertexShader(shaderDir));
			break;
		case TF_SHADER_TYPE_VG:
			printf("Loading VG shader program '%s'\n", shaderDir.c_str());
			shaders.push_back(LoadVertexShader(shaderDir));
			shaders.push_back(LoadGeometryShader(shaderDir));
			break;
		default:
			std::cerr << "Error: Unrecognized transform feedback shader type\n";
			exit(-1);
		}

		GLuint program = glCreateProgram();

		// attach all loaded shaders
		for (std::vector<GLuint>::iterator it = shaders.begin();
			it != shaders.end(); it++)
		{
			glAttachShader(program, *it);
		}

		// tell OpenGL which output attributes to capture in the
		// transform feedback buffer (TFB)
		glTransformFeedbackVaryings(program, numOutputs, outputs, GL_INTERLEAVED_ATTRIBS);

		// check if linking was successful
		glLinkProgram(program);
		GLint result = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &result);

		// if linking did not succeed, print the error message
		if (!result)
		{
			fprintf(stderr, "---> ERROR: linking shader program:\n");
			int logLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
			std::vector<GLchar> programError((logLength > 1) ? logLength : 1);
			glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
			std::cout << &programError[0] << std::endl;
		}

		// perform cleanup
		for (std::vector<GLuint>::iterator it = shaders.begin();
			it != shaders.end(); it++)
		{
			glDeleteShader(*it);
		}

		

		return program;
	}

	GLuint LoadShaderProgram(const char* path, ShaderType type)
	{
		std::vector<GLuint> shaders;
		const std::string shaderDir = Core::FileIO::getPlatformPath(path);

		switch (type)
		{
		case SHADER_TYPE_VF:
			printf("Loading VF shader program '%s'\n", shaderDir.c_str());
			shaders.push_back(LoadVertexShader(shaderDir));
			shaders.push_back(LoadFragmentShader(shaderDir));
			break;
		case SHADER_TYPE_VGF:
			printf("Loading VGF shader program '%s'\n", shaderDir.c_str());
			shaders.push_back(LoadVertexShader(shaderDir));
			shaders.push_back(LoadGeometryShader(shaderDir));
			shaders.push_back(LoadFragmentShader(shaderDir));
			break;
		default:
			std::cerr << "Error: Unrecognized shader type\n";
			exit(-1);
		}

		GLuint program = glCreateProgram();

		// attach all loaded shaders
		for (std::vector<GLuint>::iterator it = shaders.begin();
			it != shaders.end(); it++)
		{
			glAttachShader(program, *it);
		}

		// check if linking was successful
		glLinkProgram(program);
		GLint result = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &result);

		// if linking did not succeed, print the error message
		if (!result)
		{
			fprintf(stderr, "---> ERROR: linking shader program:\n");
			int logLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
			std::vector<GLchar> programError((logLength > 1) ? logLength : 1);
			glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
			std::cout << &programError[0] << std::endl;
		}

		// perform cleanup
		for (std::vector<GLuint>::iterator it = shaders.begin();
			it != shaders.end(); it++)
		{
			glDeleteShader(*it);
		}

		// check if uniforms are linked and used correctly
		std::map<std::string, std::string> uniforms;
		char uniformType[20];
		char uniformName[50];
		uniformType[19] = '\0';
		uniformName[49] = '\0';
		for (const auto& entry : std::filesystem::directory_iterator(shaderDir))
		{
			std::ifstream file;
			file.open(entry.path());

			std::string line;
			while (std::getline(file, line))
			{
				int i = sscanf(line.c_str(), "uniform %s %s;\n",
					uniformType, uniformName);
				if (i == 2)
				{
					std::string name = uniformName;
					uniformName[name.find_first_of(";")] = '\0';
					uniforms[uniformName] = uniformType;
				}
			}
			file.close();
		}
		for (auto it = uniforms.begin(); it != uniforms.end(); it++)
		{
			if (glGetUniformLocation(program, it->first.c_str()) < 0)
			{
				printf("---> WARNING: '%s' of type %s is not an active uniform!\n",
					it->first.c_str(), it->second.c_str());
			}
		}

		// done
		return program;
	}
}