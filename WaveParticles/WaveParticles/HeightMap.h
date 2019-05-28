#pragma once

// CUSTOM
#include "OpenGL.h"

// STANDARD
#include <cassert>


namespace Terrain
{
	typedef enum {
		HEIGHT_FIELD_TYPE_OCEAN,
		HEIGHT_FIELD_TYPE_SHALLOW_WATER,
		HEIGHT_FIELD_TYPE_LAND,
		// create more?
	} HeightMapFieldType;

	class HeightMapField
	{
	public:
		GLfloat Height;
		HeightMapFieldType Type;
	};
	
	class HeightMap
	{
	private:
		HeightMapField* _heightMap;
		unsigned int _size;

	public:
		HeightMap(unsigned int size)
		{
			_heightMap = new HeightMapField[size * size];
			_size = size;
		}
		~HeightMap()
		{
			delete _heightMap;
		}

		int GetSize() const
		{
			return _size;
		}

		const HeightMapField* GetHeightMap() const
		{
			return _heightMap;
		}

		HeightMapField& operator() (unsigned int x, unsigned int y)
		{
			assert(x < _size);
			assert(y < _size);
			return _heightMap[x * _size + y];
		}

		const HeightMapField& operator() (unsigned int x, unsigned int y) const
		{
			assert(x < _size);
			assert(y < _size);
			return _heightMap[x * _size + y];
		}
	};
}