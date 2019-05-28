#pragma once

#include "OpenGL.h"
#include <iostream>

class WaveParticle
{
private:

public:
	glm::vec2 Position;  // x_i
	glm::vec2 Direction; // \hat{u}_i
	glm::vec4 Color;

	/*
	GLfloat WaveLength;  // l_i
	GLfloat Amplitude;   // a_i
	GLfloat Radius;      // r_i
	GLfloat Speed;       // v_i
	*/

	void Print()
	{
		std::cout << "Position: " << glm::to_string(Position) << std::endl
			      << "Direction: " << glm::to_string(Direction) << std::endl
			;
	}
};
