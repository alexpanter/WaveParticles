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
#include "Particle.h"
#include "RandomGenerator.h"
#include "HeightMap.h"
#include "TerrainMesh.h"
#include "WaterMesh.h"

using namespace Core;
using namespace Utilities;


// WINDOW, CAMERA, AND KEYBOARD
Graphics::ApplicationWindow* win;
Graphics::BasicRTSCamera* cam;
bool keyboard[1024];
GLint waterSurfacePolygonMode = GL_LINE;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	static bool wireframe = false;
	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			win->CloseWindow();
			break;
		case GLFW_KEY_SPACE:
			if(wireframe) waterSurfacePolygonMode = GL_LINE;
			else waterSurfacePolygonMode = GL_FILL;
			wireframe = !wireframe;
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
	win->SetClearFlags(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// OPENGL
	OpenGL::PrintRendererInfo();
	//OpenGL::ApplyOpenGLRenderingSettings();

	// CAMERA
	glm::vec3 camPos(0.0f, 0.0f, 15.3325f);
	glm::vec3 camFront(0.0f, 1.0f, -1.0f);
	glm::vec3 camUp(0.0f, 1.0f, 1.0f);
	cam = new Graphics::BasicRTSCamera(aspect.GetWidth(), aspect.GetHeight(),
		camPos, camFront, camUp);

	// TIMER
	Utilities::MainTimer timer(60, 30);
	GLfloat lastTime = glfwGetTime();
	GLfloat nowTime;
	GLfloat deltaTime;





	// WAVE PARTICLE DISTRIBUTION TEXTURE

	// framebuffer target
	GLuint wpdTextureFramebuffer;
	glGenFramebuffers(1, &wpdTextureFramebuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, wpdTextureFramebuffer);

	// to complete the framebuffer, we need:
	//   - at least one attachment (color, depth, or stencil)
	//   - at least one color attachment
	//   - all attachments should be complete, with reserved memory
	//   - each buffer should have the same number of samples (for multi-sampling)

	// define texture as resolution N x N
	const int WPD_TEXTURE_SIZE = 32;

	GLuint wpdTexture;
	glGenTextures(1, &wpdTexture);
	glBindTexture(GL_TEXTURE_2D, wpdTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WPD_TEXTURE_SIZE, WPD_TEXTURE_SIZE, 0,
		GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, wpdTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
		
	glBindFramebuffer(GL_FRAMEBUFFER, 0);





	// WAVE PARTICLE DISTRIBUTION TEXTURE - CLEANUP
	Shaders::ShaderWrapper wpdTextureCleanupShader(
		"..|shaders|waveParticles|distributionTextureCleanup", Shaders::SHADER_TYPE_VF);

	GLfloat wpdTextureCleanupQuadData[] = {
		// lower-left triangle
		-1.0f, 1.0f,
		1.0f, 1.0f,
		-1.0f, -1.0f,
		// upper-right triangle
		1.0f, 1.0f,
		1.0f, -1.0f,
		-1.0f, -1.0f
	};
	GLuint wpdTextureVAO;
	GLuint wpdTextureVBO;
	glGenVertexArrays(1, &wpdTextureVAO);
	glBindVertexArray(wpdTextureVAO);

	glGenBuffers(1, &wpdTextureVBO);
	glBindBuffer(GL_ARRAY_BUFFER, wpdTextureVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(wpdTextureCleanupQuadData),
		wpdTextureCleanupQuadData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);



	// WAVE PARTICLE DISTRIBUTION TEXTURE - BLENDING
	Shaders::ShaderWrapper wpdTextureParticleBlendingShader(
		"..|shaders|waveParticles|particleBlending", Shaders::SHADER_TYPE_VF);

	/*glm::mat4 projection = glm::ortho(0, WPD_TEXTURE_SIZE, 0, WPD_TEXTURE_SIZE);
	wpdTextureParticleBlendingShader.Activate();
	wpdTextureParticleBlendingShader.SetUniform("projection", &projection);
	wpdTextureParticleBlendingShader.Deactivate();*/




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

	const int MAX_PARTICLES = 100000;
	const int NUM_PARTICLES = 1;
	PackedWaveParticle data[NUM_PARTICLES];
	const GLsizeiptr numParticlesSize = NUM_PARTICLES * sizeof(PackedWaveParticle);
	const GLsizeiptr particleBufferSize = MAX_PARTICLES * sizeof(PackedWaveParticle);

	for (int i = 0; i < NUM_PARTICLES; i++)
	{
		GLfloat posX = Random::NextFloat(-1.0f, 1.0f);
		GLfloat posY = Random::NextFloat(-1.0f, 1.0f);

		data[i].paramVec1 = glm::vec4(posX, posY, 0, glm::pi<GLfloat>() * 2.0f);
		data[i].paramVec2 = glm::vec4(posX, posY, glfwGetTime(), 0.3f * 0.5f);
		data[i].paramVec3 = glm::vec4(0.025f, 0.1f, 0.0f, 0.0f);
	}

	
	/* COMMENT BACK TO CREATE 5 STATICALLY POSITIONED PARTICLES
	// PARTICLE 1
	// (Position.x, Position.y, PropagationAngle, DispersionAngle)
	data[0].paramVec1 = glm::vec4(0.0f, 0.0f, 0, glm::pi<GLfloat>() * 2.0f);
	// (Origin.x, Origin.y, TimeAtOrigin, Velocity / AmplitudeSign)
	data[0].paramVec2 = glm::vec4(0.0f, 0.0f, glfwGetTime(), 0.3f);
	// (Radius, Amplitude, nBorderFrames)
	data[0].paramVec3 = glm::vec4(0.01f, 0.1f, 0.0f, 0.0f); // TODO: Radius in NDC ??

	// PARTICLE 2
	data[1].paramVec1 = glm::vec4(-0.5f, 0.5f, 0, glm::pi<GLfloat>() * 2.0f);
	data[1].paramVec2 = glm::vec4(-0.5f, 0.5f, glfwGetTime(), 0.3f);
	data[1].paramVec3 = glm::vec4(0.01f, 0.1f, 0.0f, 0.0f);

	// PARTICLE 3
	data[2].paramVec1 = glm::vec4(0.5f, 0.5f, 0, glm::pi<GLfloat>() * 2.0f);
	data[2].paramVec2 = glm::vec4(0.5f, 0.5f, glfwGetTime(), 0.3f);
	data[2].paramVec3 = glm::vec4(0.01f, 0.1f, 0.0f, 0.0f);

	// PARTICLE 4
	data[3].paramVec1 = glm::vec4(0.5f, -0.5f, 0, glm::pi<GLfloat>() * 2.0f);
	data[3].paramVec2 = glm::vec4(0.5f, -0.5f, glfwGetTime(), 0.3f);
	data[3].paramVec3 = glm::vec4(0.01f, 0.1f, 0.0f, 0.0f);

	// PARTICLE 5
	data[4].paramVec1 = glm::vec4(-0.5f, -0.5f, 0, glm::pi<GLfloat>() * 2.0f);
	data[4].paramVec2 = glm::vec4(-0.5f, -0.5f, glfwGetTime(), 0.3f);
	data[4].paramVec3 = glm::vec4(0.01f, 0.1f, 0.0f, 0.0f);
	*/



	glGenBuffers(2, tbo);
	glBindBuffer(GL_ARRAY_BUFFER, tbo[read]);
	glBufferData(GL_ARRAY_BUFFER, particleBufferSize, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, numParticlesSize, data);

	// void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data);
	//
	// target : Specifies the target buffer object.The symbolic constant must be 
	//          GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_PIXEL_PACK_BUFFER, or
	//          GL_PIXEL_UNPACK_BUFFER.
	// offset : Specifies the offset into the buffer object's data store where data
	//          replacement will begin, measured in bytes.
	// size : Specifies the size in bytes of the data store region being replaced.
	// data : Specifies a pointer to the new data that will be copied into the data store.


	glBindBuffer(GL_ARRAY_BUFFER, tbo[write]);
	glBufferData(GL_ARRAY_BUFFER, particleBufferSize, nullptr, GL_STATIC_DRAW);

	// query Transform Feedback (TF) number of particles output
	GLuint nParticlesAlive = NUM_PARTICLES;
	GLuint nParticlesAliveQueryObject;
	glGenQueries(1, &nParticlesAliveQueryObject);

	// query TF shader running time
	GLint64 timeElapsedTFShader = 0;
	double timeElapsedMilliseconds = 0;
	GLuint timeElapsedTFShaderQueryObject;
	glGenQueries(1, &timeElapsedTFShaderQueryObject);

	glBindVertexArray(0);



	// HEIGHT MAP FOR WATER SURFACE VISUALIZATION

	// same size as Wave Particle Distribution Texture
	Terrain::WaterMesh* waterSurfaceMesh = new Terrain::WaterMesh(WPD_TEXTURE_SIZE);

	Shaders::ShaderWrapper waterSurfaceMeshShader("..|shaders|waveParticles|waterSurface",
		Shaders::SHADER_TYPE_VF);

	waterSurfaceMeshShader.Activate();

	cam->CalculateViewProjection();
	glm::mat4 viewProjection(1.0f);
	viewProjection *= *(cam->GetProjectionMatrix());
	viewProjection *= *(cam->GetViewMatrix());
	waterSurfaceMeshShader.SetUniform("viewProjection", &viewProjection);
	waterSurfaceMeshShader.SetUniform("mapSize", (GLfloat)WPD_TEXTURE_SIZE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, wpdTexture);
	waterSurfaceMeshShader.SetUniformTexture("wpdTexture", 0);

	waterSurfaceMeshShader.Deactivate();


	// GAME LOOP
	while (win->IsRunning())
	{
		timer.MeasureTime();

		bool camUpdate = false;
		while (timer.ShouldUpdate())
		{
			nowTime = glfwGetTime();
			deltaTime = nowTime - lastTime;
			lastTime = nowTime;
			win->PollEvents();
			if (MoveCamera(deltaTime)) { camUpdate = true; }
		}

		if (camUpdate) {
			waterSurfaceMeshShader.Activate();

			cam->CalculateViewProjection();
			viewProjection = glm::mat4(1.0f);
			viewProjection *= *(cam->GetProjectionMatrix());
			viewProjection *= *(cam->GetViewMatrix());
			waterSurfaceMeshShader.SetUniform("viewProjection", &viewProjection);

			waterSurfaceMeshShader.Deactivate();
		}
		
		if (timer.ShouldRender()) {
			win->ClearWindow();



			// PERFORM TRANSFORM FEEDBACK
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glBindVertexArray(vao);

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

			glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, nParticlesAliveQueryObject);
			glBeginQuery(GL_TIME_ELAPSED, timeElapsedTFShaderQueryObject);
			{
				glBeginTransformFeedback(GL_POINTS);
				glDrawArrays(GL_POINTS, 0, nParticlesAlive);
				glEndTransformFeedback();
			}
			glEndQuery(GL_TIME_ELAPSED);
			glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

			glGetQueryObjectuiv(nParticlesAliveQueryObject, GL_QUERY_RESULT, &nParticlesAlive);
			glGetQueryObjecti64v(timeElapsedTFShaderQueryObject, GL_QUERY_RESULT,
				&timeElapsedTFShader);

			glDisable(GL_RASTERIZER_DISCARD);
			glFlush();
			imageTFShader.Deactivate();

			glBindVertexArray(0);




			// CLEAN WAVE PARTICLE DISTRIBUTION TEXTURE FROM PREVIOUS RENDERING CALLS
			glViewport(0, 0, WPD_TEXTURE_SIZE, WPD_TEXTURE_SIZE);

			glBindFramebuffer(GL_FRAMEBUFFER, wpdTextureFramebuffer);

			wpdTextureCleanupShader.Activate();
			glBindVertexArray(wpdTextureVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			wpdTextureCleanupShader.Deactivate();
			glFlush();




			// RENDER WAVE PARTICLE DISTRIBUTION TEXTURE

			// set attribute pointers
			glBindVertexArray(vao);

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

			// Enable additive blending, such that fragments are not overwritten
			// but blended (added) together
			GLboolean isBlendingEnabled;
			glGetBooleanv(GL_BLEND, &isBlendingEnabled);

			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);


			// now particles can be rendered onto the 'wave particle distribution texture',
			// which for each texel contains information about neighbouring particles
			wpdTextureParticleBlendingShader.Activate();

			glDrawArrays(GL_POINTS, 0, nParticlesAlive);

			wpdTextureParticleBlendingShader.Deactivate();

			glBindVertexArray(0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_BLEND);




			// RENDER TRANSFORM FEEDBACK RESULT
			glViewport(0, 0, aspect.GetWidth(), aspect.GetHeight());

			///*
			glBindVertexArray(vao);

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
			glDrawArrays(GL_POINTS, 0, nParticlesAlive);
			visualizeShader.Deactivate();

			glBindVertexArray(0);
			//*/


			// RENDER WATER SURFACE
			glPolygonMode(GL_FRONT_AND_BACK, waterSurfacePolygonMode);

			waterSurfaceMeshShader.Activate();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, wpdTexture);
			waterSurfaceMesh->Render();
			waterSurfaceMeshShader.Deactivate();

			// DOUBLE_BUFFERING
			win->SwapBuffers();

			// SWAPPING TF BUFFERS
			int temp = read;
			read = write;
			write = temp;
		}

		if (timer.ShouldReset()) {
			timeElapsedMilliseconds = timeElapsedTFShader / 1000000.0;
			win->SetTitle(timer.GetTimeTitle() + " | particles alive: "
				+ std::to_string(nParticlesAlive)
				+ " | TF shader time (ms): " + std::to_string(timeElapsedMilliseconds));
			std::cout << win->GetTitle() << std::endl;
		}
	}

	// cleanup
	delete waterSurfaceMesh;
	glDeleteQueries(1, &nParticlesAliveQueryObject);
	glDeleteBuffers(2, tbo);
	glDeleteVertexArrays(1, &vao);

	delete cam;
	delete win;
	return 0;
}
