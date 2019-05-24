#pragma once

// CUSTOM
#include "OpenGL.h"


namespace Graphics
{
	class Image
	{
	private:
		GLuint VBO;
		GLuint VAO;
		GLfloat vertexData[16] = {
			// vertex-pos   tex-coord
			0.0f, 0.0f, 0.0f, 1.0f,   // lower-left
			0.0f, 0.0f, 0.0f, 0.0f,   // upper-left
			0.0f, 0.0f, 1.0f, 0.0f,   // upper-right
			0.0f, 0.0f, 1.0f, 1.0f    // lower-right
		};

	public:
		Image(glm::vec2 lowerLeft, GLfloat width, GLfloat height)
		{
			// upper-left
			vertexData[0] = lowerLeft.x;
			vertexData[1] = lowerLeft.y + height;

			// lower-left
			vertexData[4] = lowerLeft.x;
			vertexData[5] = lowerLeft.y;

			// lower-right
			vertexData[8] = lowerLeft.x + width;
			vertexData[9] = lowerLeft.y;

			// upper-right
			vertexData[12] = lowerLeft.x + width;
			vertexData[13] = lowerLeft.y + height;

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

			// vertex position
			// (GLuint index, GLint size, GLenum type, GLboolean normalized,
			//  GLsizei stride, const GLvoid * pointer);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
				(GLvoid*)0);
			glEnableVertexAttribArray(0);

			// texture coordinate
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
				(GLvoid*)(2 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);

			glBindVertexArray(0);
		}
		~Image()
		{
			// to prevent GPU memory leaks!
			glBindVertexArray(VAO);
			glDeleteBuffers(1, &VBO);
			glBindVertexArray(0);
			glDeleteVertexArrays(1, &VAO);
		}

		void Draw()
		{
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			glBindVertexArray(0);
		}
	};
}