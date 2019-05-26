// disable stupid compile errors with sscanf unsafe
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

using namespace Core;


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
	Graphics::AspectRatio aspect(500, Graphics::ASPECT_RATIO_4_3);
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
	const GLchar* imageTFShaderOutputs[] = { "NewPosition" };
	Shaders::ShaderWrapper imageTFShader("..|shaders|movePoint",
		Shaders::TF_SHADER_TYPE_V, imageTFShaderOutputs, 1);
	imageTFShader.Activate();

	int read = 0;
	int write = 1;

	GLuint vao;
	GLuint tbo[2];

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glm::vec2 data[] = { glm::vec2(0.0f, 0.0f) };
	printf("DATA: (%f, %f)\n", data[0].x, data[0].y);

	glGenBuffers(2, tbo);
	glBindBuffer(GL_ARRAY_BUFFER, tbo[read]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, tbo[write]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), nullptr, GL_STATIC_DRAW);
	

	GLint inputAttrib = glGetAttribLocation(imageTFShader.GetShader(), "Position");
	//glEnableVertexAttribArray(inputAttrib);
	//glVertexAttribPointer(inputAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);


	// SETUP VAO FOR RENDERING TF RESULT
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);
	//glBindVertexArray(vao[1]);
	//glBindBuffer(GL_ARRAY_BUFFER, tbo);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	//glEnableVertexAttribArray(0);


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
			imageTFShader.Activate();
			glBindBuffer(GL_ARRAY_BUFFER, tbo[read]);
			glEnableVertexAttribArray(inputAttrib);
			glVertexAttribPointer(inputAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

			glEnable(GL_RASTERIZER_DISCARD);

			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo[write]);
			glBeginTransformFeedback(GL_POINTS);
			glDrawArrays(GL_POINTS, 0, 1);
			glEndTransformFeedback();

			glDisable(GL_RASTERIZER_DISCARD);
			glFlush();
			imageTFShader.Deactivate();

			// RENDER TRANSFORM FEEDBACK RESULT
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, tbo[write]);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			visualizeShader.Activate();
			glDrawArrays(GL_POINTS, 0, 1);
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
