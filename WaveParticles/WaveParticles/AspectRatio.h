#pragma once

// STANDARD
#include <iostream>

namespace Graphics
{
	typedef enum
	{
		ASPECT_RATIO_21_9,  // cinemascope
		ASPECT_RATIO_16_9,  // HD video
		ASPECT_RATIO_16_2,  // *for testing purposes*
		ASPECT_RATIO_4_3,   // standard monitor
		ASPECT_RATIO_3_2,   // classic film
		ASPECT_RATIO_1_1    // quadratic window
	} Aspect;

	class AspectRatio
	{
	private:
		int _width;
		int _height;

	public:
		AspectRatio(int width, Aspect aspect) : _width(width)
		{
			switch (aspect)
			{
			case ASPECT_RATIO_21_9: _height = _width / 21 * 9; break;
			case ASPECT_RATIO_16_9: _height = _width / 16 * 9; break;
			case ASPECT_RATIO_16_2: _height = _width / 16 * 2; break;
			case ASPECT_RATIO_4_3: _height = _width / 4 * 3; break;
			case ASPECT_RATIO_3_2: _height = _width / 3 * 2; break;
			case ASPECT_RATIO_1_1: _height = _width; break;

			default:
				_height = 0;
				std::cerr << "Undefined aspect ratio enum value!" << std::endl;
				break;
			}
		}

		int GetWidth() { return _width; }
		int GetHeight() { return _height; }
	};
}