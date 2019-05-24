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

	// FILE I/O TEST
	/*std::cout << "platform separator: '" << FileIO::getPlatformSeparator() << "'" << std::endl;
	std::string contents = FileIO::getPlatformFilePath("..|shaders|image|vertex.shd");
	std::cout << "read shader: " << FileIO::readFileContents(contents) << std::endl;*/

	// SHADERS
	Shaders::ShaderWrapper imageShader("..|shaders|image", Shaders::SHADER_TYPE_VF);

	// IMAGE TEST
	glm::vec2 imageLowerLeft(-0.5, -0.5f);
	GLfloat imageWidth = 1.0f;
	GLfloat imageHeight = 1.0f;
	Graphics::Image image(imageLowerLeft, imageWidth, imageHeight);

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
		}
		
		if (timer.ShouldRender()) {
			win->ClearWindow();

			imageShader.Activate();
			image.Draw();
			imageShader.Deactivate();

			win->SwapBuffers();
		}

		if (timer.ShouldReset()) {
			win->SetTitle(timer.GetTimeTitle());
		}
	}

	delete win;
	return 0;
}
