#pragma once

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // glm::to_string()

// STANDARD
#include <iostream>
#include <map>

namespace OpenGL
{
	// OpenGL version settings
	static constexpr int YGGDRASIL_OPENGL_VERSION_MAJOR = 3;
	static constexpr int YGGDRASIL_OPENGL_VERSION_MINOR = 3;
	static constexpr int YGGDRASIL_OPENGL_PROFILE = GLFW_OPENGL_CORE_PROFILE;

	// how many samples we want. Should be either 1, 4, 8, or 16.
	static constexpr int YGGDRASIL_OPENGL_MULTISAMPLE = 1;

	void InitGLFW()
	{
		if (glfwInit() == GL_FALSE)
		{
			std::cerr << "Could not initialize GLFW!" << std::endl;
			exit(0);
		}

		// setting window hints
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, YGGDRASIL_OPENGL_VERSION_MAJOR);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, YGGDRASIL_OPENGL_VERSION_MINOR);
		glfwWindowHint(GLFW_OPENGL_PROFILE, YGGDRASIL_OPENGL_PROFILE);

		// fix compilation on MacOSX
		// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		// if multisampling has been configured
		if (YGGDRASIL_OPENGL_MULTISAMPLE > 1)
		{
			glfwWindowHint(GLFW_SAMPLES, YGGDRASIL_OPENGL_MULTISAMPLE);
		}
	}

	void InitGLAD()
	{
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			exit(-1);
		}
	}

	void ApplyOpenGLRenderingSettings()
	{
		// depth buffering
		glEnable(GL_DEPTH_TEST);

		// back-face culling
		glFrontFace(GL_CW); // triangle vertices are read clock-wise
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		// multi-sampling (automatic anti-aliasing)
		if (YGGDRASIL_OPENGL_MULTISAMPLE > 1)
		{
			glEnable(GL_MULTISAMPLE);
		}
	}

	void PrintRendererInfo()
	{
		const GLubyte* renderer = glGetString(GL_RENDERER);
		const GLubyte* version = glGetString(GL_VERSION);
		printf("Renderer: %s\n", renderer);
		printf("OpenGL version supported %s\n", version);
	}

	void PrintOpenGLHardwareStats()
	{
		std::map<GLenum, const char*> stats;
		stats[GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS] = "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS";
		stats[GL_MAX_CUBE_MAP_TEXTURE_SIZE] = "GL_MAX_CUBE_MAP_TEXTURE_SIZE";
		stats[GL_MAX_3D_TEXTURE_SIZE] = "GL_MAX_3D_TEXTURE_SIZE";
		stats[GL_MAX_TEXTURE_SIZE] = "GL_MAX_TEXTURE_SIZE";
		stats[GL_MAX_TEXTURE_BUFFER_SIZE] = "GL_MAX_TEXTURE_BUFFER_SIZE";
		stats[GL_MAX_TEXTURE_UNITS] = "GL_MAX_TEXTURE_UNITS";
		stats[GL_MAX_COLOR_ATTACHMENTS] = "GL_MAX_COLOR_ATTACHMENTS";
		stats[GL_MAX_COLOR_TEXTURE_SAMPLES] = "GL_MAX_COLOR_TEXTURE_SAMPLES";
		//stats[GL_MAX_UNIFORM_LOCATIONS] = "GL_MAX_UNIFORM_LOCATIONS";
		stats[GL_MAX_COMBINED_UNIFORM_BLOCKS] = "GL_MAX_COMBINED_UNIFORM_BLOCKS";
		stats[GL_MAX_DEPTH_TEXTURE_SAMPLES] = "GL_MAX_DEPTH_TEXTURE_SAMPLES";
		stats[GL_MAX_DRAW_BUFFERS] = "GL_MAX_DRAW_BUFFERS";
		stats[GL_MAX_RENDERBUFFER_SIZE] = "GL_MAX_RENDERBUFFER_SIZE";
		//stats[GL_MAX_VIEWPORTS] = "GL_MAX_VIEWPORTS";
		stats[GL_MAX_SAMPLES] = "GL_MAX_SAMPLES";

		for (auto const& [key, val] : stats)
		{
			int v = 0;
			glGetIntegerv(key, &v);
			printf("%s: %i\n", val, v);
		}
	}
}