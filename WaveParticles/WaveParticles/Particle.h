#pragma once

#include "OpenGL.h"

class WaveParticle
{
private:

public:
	glm::vec2 Position;  // x_i
	glm::vec2 Direction; // \hat{u}_i

	GLfloat WaveLength;  // l_i
	GLfloat Amplitude;   // a_i
	GLfloat Radius;      // r_i
	GLfloat Speed;       // v_i
};
