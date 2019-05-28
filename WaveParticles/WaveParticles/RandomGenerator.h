#pragma once

// STANDARD
#include <random>
#include <cstddef>
#include <memory> // std::unique_ptr<T>
#include <limits> // std::numeric_limits
#include <cassert>

// CUSTOM
#include "OpenGL.h"


namespace Utilities
{
	template<class T>class RandomGenerator
	{
	private:
		static RandomGenerator<T>* _instance;

		typedef typename std::conditional<
			std::is_floating_point<T>::value,
			std::uniform_real_distribution<T>,
			std::uniform_int_distribution<T>>::type dist_type;

		std::random_device* _rdev;
		std::default_random_engine* _re;

		RandomGenerator<T>() {
			_rdev = new std::random_device();
			_re = new std::default_random_engine((*_rdev)());
		}
		~RandomGenerator<T>() {
			delete _rdev;
			delete _re;
		}

	public:
		static RandomGenerator<T>& GetInstance() {
			if (!RandomGenerator<T>::_instance) {
				RandomGenerator<T>::_instance = new RandomGenerator<T>();
			}
			return *_instance;
		}

		T NextRandomValue(const T min, const T max) {
			dist_type uni(min, max); // (inclusive, inclusive)
			return static_cast<T>(uni(*_re));
		}

		// delete copy-constructor and assignment constructor
		RandomGenerator<T>(const RandomGenerator<T>&) = delete;
		RandomGenerator<T>& operator= (const RandomGenerator<T>) = delete;
	};

	template<class T>
	RandomGenerator<T>* RandomGenerator<T>::_instance = nullptr;

	// TODO: Provide standard template instantiations here !

	namespace Random
	{
		// integer values
		GLint NextInt(GLint min, GLint max) {
			if (min > max) {
				int tmp = min;
				min = max;
				max = tmp;
			}
			return RandomGenerator<GLint>::GetInstance().NextRandomValue(min, max);
		}

		GLuint NextUint(GLuint min, GLuint max) {
			if (min > max) {
				int tmp = min;
				min = max;
				max = tmp;
			}
			return RandomGenerator<GLuint>::GetInstance().NextRandomValue(min, max);
		}


		// floating-point values
		GLfloat NextFloat(GLfloat min, GLfloat max) {
			if (min > max) {
				int tmp = min;
				min = max;
				max = tmp;
			}
			return RandomGenerator<GLfloat>::GetInstance().NextRandomValue(min, max);
		}

		GLfloat NextFloat()
		{
			GLfloat min = std::numeric_limits<GLfloat>::min();
			GLfloat max = std::numeric_limits<GLfloat>::max();
			return RandomGenerator<GLfloat>::GetInstance().NextRandomValue(min, max);
		}

		GLfloat NextPositiveFloat(GLfloat max) {
			assert(max > 0.0f);
			return RandomGenerator<GLfloat>::GetInstance().NextRandomValue(0.0f, max);
		}

		GLfloat NextPositiveFloat() {
			GLfloat max = std::numeric_limits<GLfloat>::max();
			return RandomGenerator<GLfloat>::GetInstance().NextRandomValue(0.0f, max);
		}

		GLfloat NextNegativeFloat(GLfloat min) {
			assert(min < 0.0f);
			return RandomGenerator<GLfloat>::GetInstance().NextRandomValue(min, 0.0f);
		}

		GLfloat NextNegativeFloat() {
			GLfloat min = std::numeric_limits<GLfloat>::min();
			return RandomGenerator<GLfloat>::GetInstance().NextRandomValue(min, 0.0f);
		}

		GLdouble NextDouble(GLdouble min, GLdouble max) {
			if (min > max) {
				int tmp = min;
				min = max;
				max = tmp;
			}
			return RandomGenerator<GLdouble>::GetInstance().NextRandomValue(min, max);
		}


		// floating-point values clamped in the range [0,1]
		GLclampf NextClampedFloat() {
			return RandomGenerator<GLclampf>::GetInstance().NextRandomValue(0.0f, 1.0f);
		}

		GLclampd NextClampedDouble() {
			return RandomGenerator<GLclampd>::GetInstance().NextRandomValue(0.0, 1.0);
		}


		// normalized floating-point values in the range [-1,1]
		GLfloat NextNormalizedFloat() {
			return RandomGenerator<GLfloat>::GetInstance().NextRandomValue(-1.0f, 1.0f);
		}

		GLdouble NextNormalizedDouble() {
			return RandomGenerator<GLdouble>::GetInstance().NextRandomValue(-1.0, 1.0);
		}


		// random probability-based choice
		GLboolean GetRandomChoice(GLclampf probability)
		{
			return NextClampedFloat() <= probability;
		}
	}
}