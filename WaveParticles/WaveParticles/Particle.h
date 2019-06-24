#pragma once

#include "OpenGL.h"
#include "RandomGenerator.h"
#include <iostream>

using namespace Utilities;


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

class PackedWaveParticle
{
private:
	static bool propagate;

public:
	// (Position.x, Position.y, PropagationAngle, DispersionAngle)
	glm::vec4 paramVec1;

	// (Origin.x, Origin.y, TimeAtOrigin, Velocity / AmplitudeSign)
	glm::vec4 paramVec2;
	  
	// (Radius, Amplitude, nBorderFrames)
	glm::vec4 paramVec3;

	static void TogglePropagate() { propagate = !propagate; }

	static void GenerateRandom(PackedWaveParticle& particle)
	{
		GLfloat posX = Random::NextFloat(-1.0f, 1.0f);
		GLfloat posY = Random::NextFloat(-1.0f, 1.0f);

		// (Position.x, Position.y, PropagationAngle, DispersionAngle)
		particle.paramVec1 = glm::vec4(posX, posY, 0, (PackedWaveParticle::propagate) ? glm::pi<GLfloat>() * 2.0f : 0.0f);

		// (Origin.x, Origin.y, TimeAtOrigin, Velocity / AmplitudeSign)
		particle.paramVec2 = glm::vec4(posX, posY, glfwGetTime(), 0.3f * 0.5f);

		// (Radius, Amplitude, nBorderFrames)
		particle.paramVec3 = glm::vec4(0.025f, 15.0f, 0.0f, 0.0f);
	}

	/*
	* Position.x
	* Position.y
	*
	* Origin.x
	* Origin.y
	* TimeAtOrigin
	* PropagationAngle
	*
	* Amplitude
	* Radius
	* Speed / AmplitudeSign
	* DispersionAngle
	*/
};

bool PackedWaveParticle::propagate = true;