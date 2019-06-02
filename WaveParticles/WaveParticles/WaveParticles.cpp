﻿// disable stupid compile errors with sscanf unsafe
// (yes I know it is unsafe, but I take care when I use it!)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

// STANDARD
#include <iostream>

// CUSTOM
#include "ApplicationWindow.h"
#include "AspectRatio.h"
#include "Camera.h"
#include "MainTimer.h"
#include "FileIO.h"
#include "ShaderWrapper.h"
#include "Image.h"
#include "TestTransformFeedback.h"
#include "Particle.h"
#include "RandomGenerator.h"

using namespace Core;
using namespace Utilities;


// WINDOW, CAMERA, AND KEYBOARD
Graphics::ApplicationWindow* win;
Graphics::BasicRTSCamera* cam;
bool keyboard[1024];

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			win->CloseWindow();
			break;
		default:
			keyboard[key] = true;
		}
	}
	else if (action == GLFW_RELEASE) {
		keyboard[key] = false;
	}
}

bool MoveCamera(GLfloat deltaTime)
{
	GLfloat cameraSpeed = 15.0f * deltaTime;
	bool retval = false;

	if (keyboard[GLFW_KEY_W]) {
		cam->MoveNorth(cameraSpeed);
		retval = true;
	}
	else if (keyboard[GLFW_KEY_S]) {
		cam->MoveSouth(cameraSpeed);
		retval = true;
	}
	if (keyboard[GLFW_KEY_A]) {
		cam->MoveWest(cameraSpeed);
		retval = true;
	}
	else if (keyboard[GLFW_KEY_D]) {
		cam->MoveEast(cameraSpeed);
		retval = true;
	}
	if (keyboard[GLFW_KEY_Q]) {
		cam->RotateLeft(cameraSpeed / 3.0f);
		retval = true;
	}
	else if (keyboard[GLFW_KEY_E]) {
		cam->RotateRight(cameraSpeed / 3.0f);
		retval = true;
	}
	if (keyboard[GLFW_KEY_Z]) {
		if (keyboard[GLFW_KEY_LEFT_SHIFT]) {
			cam->MoveUp(cameraSpeed);
		}
		else {
			cam->MoveDown(cameraSpeed);
		}
		retval = true;
	}
	return retval;
}

int main()
{
	// WINDOW SETUP
	win = new Graphics::ApplicationWindow();
	Graphics::AspectRatio aspect(800, Graphics::ASPECT_RATIO_1_1);
	win->SetTitle("Wave Particles");
	win->SetWindowed(aspect);
	win->SetKeyCallback(KeyCallback);
	
	// OPENGL
	OpenGL::PrintRendererInfo();

	// CAMERA
	glm::vec3 camPos(0.0f, 0.0f, 10.0f);
	glm::vec3 camFront(1.0f, 1.0f, 0.0f);
	glm::vec3 camUp(0.0f, 0.0f, 1.0f);
	cam = new Graphics::BasicRTSCamera(aspect.GetWidth(), aspect.GetHeight(),
		camPos, camFront, camUp);

	// TIMER
	Utilities::MainTimer timer(30, 30);


	// DATA FOR TRANSFORM FEEDBACK

	// visualizing shader
	Shaders::ShaderWrapper visualizeShader("..|shaders|point", Shaders::SHADER_TYPE_VGF);

	// TF shader
	const GLchar* imageTFShaderOutputs[] = { 
		"paramVec1", "paramVec2", "paramVec3" 
	};
	Shaders::ShaderWrapper imageTFShader("..|shaders|waveParticles|particlePropagation",
		Shaders::TF_SHADER_TYPE_VG, imageTFShaderOutputs, 3);
	imageTFShader.Activate();

	int read = 0;
	int write = 1;

	GLuint vao;
	GLuint tbo[2];

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	const int MAX_PARTICLES = 10000;
	const int NUM_PARTICLES = 1;
	PackedWaveParticle data[NUM_PARTICLES];
	
	// (Position.x, Position.y, PropagationAngle, DispersionAngle)
	data[0].paramVec1 = glm::vec4(0.0f, 0.0f, glm::pi<GLfloat>() * 0.25f, glm::pi<GLfloat>());
	// (Origin.x, Origin.y, TimeAtOrigin, Velocity / AmplitudeSign)
	data[0].paramVec2 = glm::vec4(0.0f, 0.0f, glfwGetTime(), 0.001f);
	// (Radius, Amplitude, nBorderFrames)
	data[0].paramVec3 = glm::vec4(0.1f, 0.1f, 0.0f, 0.0f); // TODO: Radius in NDC instead??


	glGenBuffers(2, tbo);
	glBindBuffer(GL_ARRAY_BUFFER, tbo[read]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, tbo[write]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), nullptr, GL_STATIC_DRAW);


	// GAME LOOP
	while (win->IsRunning())
	{
		timer.MeasureTime();

		bool camUpdate = false;
		while (timer.ShouldUpdate())
		{
			win->PollEvents();
			if (MoveCamera(timer.GetDeltaTime())) { camUpdate = true; }
		}

		if (camUpdate) {
			cam->CalculateViewProjection();
			std::cout << "Move camera!" << std::endl;
		}
		
		if (timer.ShouldRender()) {
			win->ClearWindow();

			// PERFORM TRANSFORM FEEDBACK

			// OBS: Remember to set gl point size so that several fragments are
			// generated for each particle!

			imageTFShader.Activate();
			imageTFShader.SetUniform("time", (GLfloat)glfwGetTime());
			glBindBuffer(GL_ARRAY_BUFFER, tbo[read]);

			// (Position.x, Position.y, PropagationAngle, DispersionAngle)
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(PackedWaveParticle),
				(GLvoid*)0);
			glEnableVertexAttribArray(0);

			// (Origin.x, Origin.y, TimeAtOrigin, Velocity / AmplitudeSign)
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(PackedWaveParticle),
				(GLvoid*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(1);

			// (Radius, Amplitude, nBorderFrames)
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(PackedWaveParticle),
				(GLvoid*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(2);

			glEnable(GL_RASTERIZER_DISCARD);

			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo[write]);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, tbo[write]);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, tbo[write]);

			GLuint queryObject;
			GLuint primitivesWritten = 0;
			glGenQueries(1, &queryObject);
			glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, queryObject);

			glBeginTransformFeedback(GL_POINTS);
			glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
			glEndTransformFeedback();

			glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
			glGetQueryObjectuiv(queryObject, GL_QUERY_RESULT, &primitivesWritten);
			glDeleteQueries(1, &queryObject);

			glDisable(GL_RASTERIZER_DISCARD);
			glFlush();
			imageTFShader.Deactivate();

			//std::cout << "primitives written: " << primitivesWritten << std::endl;


			// Enable additive blending, such that fragments are not overwritten
			// but blended (added) together
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);

			// now particles can be rendered onto the 'wave particle distribution texture',
			// which for each texel contains information about neighbouring particles





			// RENDER TRANSFORM FEEDBACK RESULT
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, tbo[write]);

			// (Position.x, Position.y, PropagationAngle, DispersionAngle)
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(PackedWaveParticle),
				(GLvoid*)0);
			glEnableVertexAttribArray(0);

			// (Origin.x, Origin.y, TimeAtOrigin, Velocity / AmplitudeSign)
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(PackedWaveParticle),
				(GLvoid*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(1);

			// (Radius, Amplitude, nBorderFrames)
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(PackedWaveParticle),
				(GLvoid*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(2);

			visualizeShader.Activate();
			glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
			visualizeShader.Deactivate();

			// DOUBLE_BUFFERING
			win->SwapBuffers();

			// SWAPPING TF BUFFERS
			int temp = read;
			read = write;
			write = temp;
		}

		if (timer.ShouldReset()) {
			win->SetTitle(timer.GetTimeTitle());
		}
	}

	// cleanup
	glDeleteBuffers(2, tbo);
	glDeleteVertexArrays(1, &vao);

	delete win;
	return 0;
}
