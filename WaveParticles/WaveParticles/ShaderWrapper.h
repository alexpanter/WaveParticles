#pragma once

// CUSTOM
#include "ShaderLoader.h"
#include "ShaderType.h"


namespace Core::Shaders
{
	// wrapper class for the functions above
	class ShaderWrapper {
	private:
		GLuint _shader;
		GLint64 _deltaTime = 0;
		GLint64 _lastTime = 0;
		const std::string _shaderName;

	protected:
	public:
		// TODO: Eliminate uses of char*, and use std::string& instead!
		ShaderWrapper(const char* path, ShaderType type)
			: _shaderName(path)
		{
			_shader = LoadShaderProgram(path, type);
		}

		ShaderWrapper(const char* path, TransformFeedbackShaderType type,
			const std::vector<std::string>& outputs)
			: _shaderName(path)
		{
			_shader = LoadTransformFeedbackShaderProgram(path, type,
				outputs);
		}

		~ShaderWrapper()
		{
			glDeleteShader(_shader); // is this needed ?
			glUseProgram(0);
		}

		void Activate()
		{
			glGetInteger64v(GL_TIMESTAMP, &_lastTime);
			glUseProgram(_shader);
		}
		void Deactivate()
		{
			glUseProgram(0);
			GLint64 nowTime;
			glGetInteger64v(GL_TIMESTAMP, &nowTime);
			_deltaTime = nowTime - _lastTime;
		}

		void PrintLastMeasuredTime()
		{
			double milliseconds = _deltaTime / 1000000.0;
			printf("Shader time (%s) = %gms  => %g FPS\n",
				_shaderName.c_str(), milliseconds, 1.0 / milliseconds);
			fflush(stdout);
		}

		// the 'number' is an integer between 0 and
		// GL_MAX_TEXTURE_UNITS (probably 16)
		void SetUniformTexture(const char* name, GLuint number)
		{
			// TODO: is it ok to compare this way, or do we need to:
			// glGetIntegerv(GL_MAX_TEXTURE_UNITS, &someIntVariable) ?
			if (!(number < GL_MAX_TEXTURE_UNITS))
			{
				std::cerr << "ShaderWrapper::SetUniformTexture: "
					<< "Invalid texture number (" << number
					<< ")" << std::endl;
			}

			GLint location = glGetUniformLocation(_shader, name);
			glUniform1i(location, number);
		}

		void SetUniform(const char* name, const glm::mat4* mat)
		{
			GLint location = glGetUniformLocation(_shader, name);
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(*mat));
		}

		// vector is already allocated
		void SetUniform(const char* name, const glm::vec2* vec)
		{
			GLint location = glGetUniformLocation(_shader, name);
			glUniform2fv(location, 1, glm::value_ptr(*vec));
		}

		// vector can be called by value, i.e. not allocated prior to
		// calling this function
		void SetUniform(const char* name, const glm::vec2& vec)
		{
			GLint location = glGetUniformLocation(_shader, name);
			glUniform2fv(location, 1, glm::value_ptr(vec));
		}

		// vector is already allocated
		void SetUniform(const char* name, const glm::vec3* vec)
		{
			GLint location = glGetUniformLocation(_shader, name);
			glUniform3fv(location, 1, glm::value_ptr(*vec));
		}

		// vector can be called by value, i.e. not allocated prior to
		// calling this function
		void SetUniform(const char* name, const glm::vec3& vec)
		{
			GLint location = glGetUniformLocation(_shader, name);
			glUniform3fv(location, 1, glm::value_ptr(vec));
		}

		// vector is already allocated
		void SetUniform(const char* name, const glm::vec4* vec)
		{
			GLint location = glGetUniformLocation(_shader, name);
			glUniform4fv(location, 1, glm::value_ptr(*vec));
		}

		// vector can be called by value, i.e. not allocated prior to
		// calling this function
		void SetUniform(const char* name, const glm::vec4& vec)
		{
			GLint location = glGetUniformLocation(_shader, name);
			glUniform4fv(location, 1, glm::value_ptr(vec));
		}

		void SetUniform(const char* name, bool b)
		{
			GLint location = glGetUniformLocation(_shader, name);
			glUniform1ui(location, b);
		}

		void SetUniform(const char* name, float f)
		{
			GLint location = glGetUniformLocation(_shader, name);
			glUniform1f(location, f);
		}

		void SetUniform(const char* name, int i)
		{
			GLint location = glGetUniformLocation(_shader, name);
			glUniform1i(location, i);
		}

		void SetUniform(const char* name, unsigned int i)
		{
			GLint location = glGetUniformLocation(_shader, name);
			glUniform1ui(location, i);
		}
	};
}