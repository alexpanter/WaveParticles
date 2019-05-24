#pragma once

#include "OpenGL.h"
#include "FileIO.h"
#include "ShaderWrapper.h"

using namespace Core;





void TestTransformFeedback1()
{
	const GLchar* imageTFShaderOutputs[] = { "NewPosition" };
	Shaders::ShaderWrapper imageTFShader("..|shaders|transformFeedback",
		Shaders::TF_SHADER_TYPE_V, imageTFShaderOutputs, 1);
	imageTFShader.Activate();

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLfloat data[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
	printf("%f %f %f %f %f\n", data[0], data[1], data[2], data[3], data[4]);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	GLint inputAttrib = glGetAttribLocation(imageTFShader.GetShader(), "Position");
	glEnableVertexAttribArray(inputAttrib);
	glVertexAttribPointer(inputAttrib, 1, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint tbo;
	glGenBuffers(1, &tbo);
	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), nullptr, GL_STATIC_READ);

	//void glTransformFeedbackVaryings(GLuint program​, GLsizei count​, const char** varyings​, GLenum bufferMode​);

	// disable the rasterizer since we do not render anything right now
	glEnable(GL_RASTERIZER_DISCARD);

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, 5);
	glEndTransformFeedback();

	// make sure drawing call has been completed before accessing the results
	//
	// "However, when using double buffering, glFlush has practically no effect at all,
	// since the changes won't be visible until you swap the buffers."
	// https://stackoverflow.com/questions/2143240/opengl-glflush-vs-glfinish
	//
	glFlush();

	// to retrieve the data, copy it to an array
	GLfloat feedback[5];
	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);

	glFlush();

	// print results to test
	printf("%g %g %g %g %g\n", feedback[0], feedback[1], feedback[2],
		feedback[3], feedback[4]);

	glDisable(GL_RASTERIZER_DISCARD);

	// cleanup
	glDeleteBuffers(1, &tbo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}





void TestTransformFeedback2()
{
	const GLchar* imageTFShaderOutputs[] = { "NewGeometryPosition" };
	Shaders::ShaderWrapper imageTFShader("..|shaders|transformFeedback2",
		Shaders::TF_SHADER_TYPE_VG, imageTFShaderOutputs, 1);
	imageTFShader.Activate();

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLfloat data[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
	printf("%f %f %f %f %f\n", data[0], data[1], data[2], data[3], data[4]);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	GLint inputAttrib = glGetAttribLocation(imageTFShader.GetShader(), "Position");
	glEnableVertexAttribArray(inputAttrib);
	glVertexAttribPointer(inputAttrib, 1, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint tbo;
	glGenBuffers(1, &tbo);
	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data) * 3, nullptr, GL_STATIC_READ);

	//void glTransformFeedbackVaryings(GLuint program​, GLsizei count​, const char** varyings​, GLenum bufferMode​);

	// disable the rasterizer since we do not render anything right now
	glEnable(GL_RASTERIZER_DISCARD);

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, 5);
	glEndTransformFeedback();

	// make sure drawing call has been completed before accessing the results
	//
	// "However, when using double buffering, glFlush has practically no effect at all,
	// since the changes won't be visible until you swap the buffers."
	// https://stackoverflow.com/questions/2143240/opengl-glflush-vs-glfinish
	//
	glFlush();

	// to retrieve the data, copy it to an array
	GLfloat feedback[5 * 3];
	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);

	glFlush();

	// print results to test
	for (int i = 0; i < 15; i += 3)
	{
		printf("%g %g %g\n", feedback[i], feedback[i + 1], feedback[i + 2]);
	}

	glDisable(GL_RASTERIZER_DISCARD);

	// cleanup
	glDeleteBuffers(1, &tbo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}





void TestTransformFeedback3()
{
	const GLchar* imageTFShaderOutputs[] = { "NewPosition" };
	Shaders::ShaderWrapper imageTFShader("..|shaders|transformFeedback3",
		Shaders::TF_SHADER_TYPE_V, imageTFShaderOutputs, 1);
	imageTFShader.Activate();

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glm::vec2 data[] = { glm::vec2(0.0f, 0.0f) };
	printf("DATA: (%f, %f)\n", data[0].x, data[0].y);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	GLint inputAttrib = glGetAttribLocation(imageTFShader.GetShader(), "Position");
	glEnableVertexAttribArray(inputAttrib);
	glVertexAttribPointer(inputAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint tbo;
	glGenBuffers(1, &tbo);
	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), nullptr, GL_STATIC_READ);

	//void glTransformFeedbackVaryings(GLuint program​, GLsizei count​, const char** varyings​, GLenum bufferMode​);

	// disable the rasterizer since we do not render anything right now
	glEnable(GL_RASTERIZER_DISCARD);

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, 1);
	glEndTransformFeedback();

	// make sure drawing call has been completed before accessing the results
	//
	// "However, when using double buffering, glFlush has practically no effect at all,
	// since the changes won't be visible until you swap the buffers."
	// https://stackoverflow.com/questions/2143240/opengl-glflush-vs-glfinish
	//
	glFlush();

	// to retrieve the data, copy it to an array
	glm::vec2 feedback[1];
	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);

	glFlush();

	// print results to test
	printf("FEEDBACK: (%f, %f)\n", feedback[0].x, feedback[0].y);

	glDisable(GL_RASTERIZER_DISCARD);

	// cleanup
	glDeleteBuffers(1, &tbo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}





void TestTransformFeedback4()
{
	//Turn rendering OFF;
	glEnable(GL_RASTERIZER_DISCARD);

	//Run first GPU pass;
	glUseProgram(m_emitProgram);
	{
		//emit particles to EmitterFeedback transform feedback object;
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_EmitterFeedback);
		glBeginTransformFeedback(GL_POINTS);
		glDrawArrays(GL_POINTS, 0, m_EmitterCount);
		glEndTransformFeedback();
	}

	//Run second and third GPU passes;
	glUseProgram(m_processProgram);
	{
		//Output particles to Current
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_Current);

		//Run second GPU pass with input from feedback object EmitterFeedback
		glDrawTransformFeedback(GL_POINTS, m_EmitterFeedback);

		//If not the first frame, then run third GPU pass with
		//the input from feedback object Previous
		if (!m_isFirstHit)
			glDrawTransformFeedback(GL_POINTS, m_Previous);

		glEndTransformFeedback();
		m_isFirstHit = false;
	}

	//Turn rendering ON;
	glDisable(GL_RASTERIZER_DISCARD);

	...

		//Render particles from feedback object Current
		glDrawTransformFeedback(GL_POINTS, m_Current);

	//Swap feedback object IDs
	swap(m_Current, m_Previous);
}
