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
	static bool createRemote;

public:
	// (Position.x, Position.y, PropagationAngle, DispersionAngle)
	glm::vec4 paramVec1;

	// (Origin.x, Origin.y, TimeAtOrigin, Velocity / AmplitudeSign)
	glm::vec4 paramVec2;
	  
	// (Radius, Amplitude, nBorderFrames)
	glm::vec4 paramVec3;

	static void TogglePropagate() { propagate = !propagate; }
	static void ToggleCreateRemote() { createRemote = !createRemote; }

	static void GenerateRandom(PackedWaveParticle& particle)
	{
		glm::vec2 pos;
		if (PackedWaveParticle::createRemote) {
			pos.x = Random::NextFloat(1.0f, 2.0f);
			pos.y = Random::NextFloat(1.0f, 2.0f);
		}
		else {
			pos.x = Random::NextFloat(-1.0f, 1.0f);
			pos.y = Random::NextFloat(-1.0f, 1.0f);
		}
		
		GLfloat propAngle = Random::NextFloat(0.0f, glm::two_pi<GLfloat>());
		GLfloat amplitudeBias = Random::NextPositiveNegativeBias();
		GLfloat amplitude = amplitudeBias * ((PackedWaveParticle::propagate) ? 15.0f : 5.0f);

		// (Position.x, Position.y, PropagationAngle, DispersionAngle)
		particle.paramVec1 = glm::vec4(pos.x, pos.y, propAngle, (PackedWaveParticle::propagate) ? glm::pi<GLfloat>() * 2.0f : 0.0f);

		// (Origin.x, Origin.y, TimeAtOrigin, Velocity / AmplitudeSign)
		particle.paramVec2 = glm::vec4(pos.x, pos.y, glfwGetTime(), 0.20f * amplitudeBias);

		// (Radius, Amplitude, nBorderFrames)
		particle.paramVec3 = glm::vec4(0.025f, amplitude, 0.0f, 0.0f);
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
bool PackedWaveParticle::createRemote = false;