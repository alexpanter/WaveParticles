///
/// Terrain Mesh
///
/// A class specifically for converting from a
/// height map of z-coordinates to a complete terrain
/// mesh, responsible for buffering to GPU, and any
/// consecutive rendering calls.
///
/// Same as TerrainMesh, but for each grid point contains
/// information about terrain height (for now).
///

#pragma once

// CUSTOM
#include "OpenGL.h"
#include "HeightMap.h"

// STANDARD
#include <vector>

// static constexpr int GRID_SIZE = 32;


namespace Terrain
{
	/*
	 * Stores a triangle mesh of a game map,
	 * in a very nice and cool format, as seen below:
	 *
	 * # - # - # - # - # - # ...  (single height points)
	 *
	 * =>
	 *
	 * #---#---#---#
	 * |  /|  /|  /|
	 * | / | / | / |
	 * |/  |/  |/  |
	 * #---#---#---#   (nice triangle mesh)
	 * |  /|  /|  /|
	 * | / | / | / |
	 * |/  |/  |/  |
	 * #---#---#---#
	 *
	 */
	class WaterMesh
	{
	private:
		class VertexData
		{
		public:
			// (x,y) = gridpoint, z = terrainHeight
			glm::vec3 GridPointData;
		};

		// map properties
		int _mapSize;

		// buffer objects
		GLuint VAO, VBO, EBO;

		// data points
		int _numVertices;
		VertexData* _vertices;

		// indices for indexed rendering
		int _numIndices;
		unsigned int _sizeIndexBuffer;
		std::vector<glm::uvec3> _indices;

		// triangle normals - can also be used for units walking around
		// triangle normals, in grids of 2
		typedef struct {
			glm::vec3 upperLeft;
			glm::vec3 lowerRight;
		} _tri_normals_grid_t;

		std::vector<_tri_normals_grid_t> _triNormals;
		int _triNormalgridSize;

		// can `coordinate` index into _triNormals
		inline bool IsValidTriNormalIndex(glm::ivec2 coord) {
			return coord.x >= 0 && coord.x < _triNormalgridSize &&
				coord.y >= 0 && coord.y < _triNormalgridSize;
		}

		void _buffer_data() {
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * _numVertices,
				_vertices, GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _sizeIndexBuffer,
				_indices.data(), GL_STATIC_DRAW);

			/*
			 * GLuint index,
			 * GLint size,
			 * GLenum type,
			 * GLboolean normalized,
			 * GLsizei stride,
			 * const GLvoid * pointer);
			 */

			 // grid point data
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
				(GLvoid*)0);
			glEnableVertexAttribArray(0);

			glBindVertexArray(0);
		}

	public:
		WaterMesh(int size)
		{
			// copy z-values and generate x and y
			_mapSize = size;
			_numVertices = _mapSize * _mapSize;
			_vertices = new VertexData[_numVertices];
			for (int row = 0; row < _mapSize; row++)
			{
				for (int col = 0; col < _mapSize; col++)
				{
					// grid point data (x, y, water height)
					int index = row * _mapSize + col;
					_vertices[index].GridPointData =
						glm::vec3((GLfloat)col, (GLfloat)row, 0.0f);
				}
			}
			_vertices[0].GridPointData.z = 5.0f;

			// triangulate
			_triNormalgridSize = _mapSize - 1;

			// compute triangle normals and rendering indices
			for (int row = 0; row < _triNormalgridSize; row++) {
				for (int col = 0; col < _triNormalgridSize; col++) {
					GLuint index = row * _mapSize + col;
					int i1 = index;
					int i2 = index + 1;
					int i3 = index + _mapSize;
					int i4 = index + _mapSize + 1;

					// upper-left triangle
					_indices.push_back(glm::uvec3(i1, i2, i3));

					// lower-right triangle
					_indices.push_back(glm::uvec3(i2, i4, i3));
				}
			}

			_numIndices = _indices.size();
			_sizeIndexBuffer = _numIndices * sizeof(glm::uvec3);
			_buffer_data();
		}
		~WaterMesh()
		{
			// delete _mapGrid;
			// to prevent GPU memory leaks!
			glBindVertexArray(VAO);
			glDeleteBuffers(1, &VBO);
			glDeleteBuffers(1, &EBO);
			glBindVertexArray(0);
			glDeleteVertexArrays(1, &VAO);
			delete _vertices;
		}

		void TestPrint() {
			std::cout << "PRINTING WATER HEIGHT MAP VERTICES..." << std::endl;
			for (int i = 0; i < _numVertices; i++) {
				std::cout << "vertex: "
					<< glm::to_string(_vertices[i].GridPointData)
					<< std::endl << std::endl;
			}

			std::cout << std::endl << "PRINTING INDICES..." << std::endl;
			for (int i = 0; i < _numIndices; i++) {
				std::cout << "triangle: " << glm::to_string(_indices[i]) << std::endl;
			}

			printf("_numVertices %i\n", _numVertices);
			printf("_numIndices %i\n", _numIndices);
			printf("_sizeIndexBuffer %i\n", _sizeIndexBuffer);
			printf("_triNormalgridSize %i\n", _triNormalgridSize);
			printf("sizeof(VertexData) %lu\n", sizeof(VertexData));
		}

		void Render() const
		{
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, _numIndices * 3, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		int GetMapSize()
		{
			return _mapSize;
		}
	};
}