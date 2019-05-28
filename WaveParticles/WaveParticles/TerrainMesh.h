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
	class TerrainMesh
	{
	private:
		class VertexData
		{
		public:
			glm::vec3 Position;
			glm::vec3 Normal;
			glm::vec3 Color;
			//HeightMapFieldType Type;
		};

		template<int T>
		class GridBlock
		{
		public:
			VertexData Vertices[T * T];
			const int Size = T;
		};

		// map properties
		int _mapSize;

		// buffer objects
		GLuint VAO, VBO, EBO;

		// TODO: use grid instead of rendering the whole map!
		// GridBlock<GRID_SIZE>* _mapGrid;

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

			 // vertex position
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
				(GLvoid*)0);
			glEnableVertexAttribArray(0);

			// vertex normal
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
				(GLvoid*)(sizeof(glm::vec3)));
			glEnableVertexAttribArray(1);

			// vertex color
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData),
				(GLvoid*)(2 * sizeof(glm::vec3)));
			glEnableVertexAttribArray(2);

			// vertex type

			glBindVertexArray(0);
		}

	public:
		TerrainMesh(const HeightMap& heightMap)
		{
			// copy z-values and generate x and y
			_mapSize = heightMap.GetSize();
			_numVertices = _mapSize * _mapSize;
			_vertices = new VertexData[_numVertices];
			for (int row = 0; row < _mapSize; row++)
			{
				for (int col = 0; col < _mapSize; col++)
				{
					int index = row * _mapSize + col;
					_vertices[index].Position =
						glm::vec3(col, row, heightMap(col, row).Height);
				}
			}

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

					_tri_normals_grid_t gridBox;

					// upper-left triangle
					_indices.push_back(glm::uvec3(i1, i2, i3));
					glm::vec3 e1 = _vertices[i2].Position - _vertices[i1].Position;
					glm::vec3 e2 = _vertices[i3].Position - _vertices[i1].Position;
					gridBox.upperLeft = glm::cross(e1, e2);

					// lower-right triangle
					_indices.push_back(glm::uvec3(i2, i4, i3));
					e1 = _vertices[i4].Position - _vertices[i2].Position;
					e2 = _vertices[i3].Position - _vertices[i2].Position;
					gridBox.lowerRight = glm::cross(e1, e2);

					// insert into tri-normal grid
					_triNormals.push_back(gridBox);
				}
			}

			// calculate vertex normals
			for (int row = 0; row < _mapSize; row++) {
				for (int col = 0; col < _mapSize; col++) {
					GLuint index = row * _mapSize + col;

					int countedTriNormals = 0;

					// if vertex has an associated tri-normal grid block in each 4 directions,
					// then it is calculated in the averaged tri-normal sum.
					// Subtracting 1 because tri-normal grid is different than vertex grid.
					//
					// upper-left grid block
					glm::ivec2 i1(col - 1, row - 1);

					if (IsValidTriNormalIndex(i1)) {
						int idx = i1.y * _triNormalgridSize + i1.x;
						_tri_normals_grid_t square1 = _triNormals[idx];
						_vertices[index].Normal += square1.lowerRight;
						countedTriNormals++;
					}

					// upper-right grid block
					glm::ivec2 i2(col, row - 1);

					if (IsValidTriNormalIndex(i2)) {
						int idx = i2.y * _triNormalgridSize + i2.x;
						_tri_normals_grid_t square2 = _triNormals[idx];
						_vertices[index].Normal += (square2.upperLeft + square2.lowerRight) / 2.0f;
						countedTriNormals++;
					}

					// lower-left grid block
					glm::ivec2 i3(col - 1, row);

					if (IsValidTriNormalIndex(i3)) {
						int idx = i3.y * _triNormalgridSize + i3.x;
						_tri_normals_grid_t square3 = _triNormals[idx];
						_vertices[index].Normal += (square3.upperLeft + square3.lowerRight) / 2.0f;
						countedTriNormals++;
					}

					// lower-right grid block
					glm::ivec2 i4(col, row);

					if (IsValidTriNormalIndex(i4)) {
						int idx = i4.y * _triNormalgridSize + i4.x;
						_tri_normals_grid_t square4 = _triNormals[idx];
						_vertices[index].Normal += square4.upperLeft;
						countedTriNormals++;
					}
					_vertices[index].Normal /= (GLfloat)countedTriNormals;
					_vertices[index].Normal = glm::normalize(_vertices[index].Normal);
				}
			}
			_numIndices = _indices.size();
			_sizeIndexBuffer = _numIndices * sizeof(glm::uvec3);
			_buffer_data();
			//
			// TODO: These steps should be performed on a per grid basis

			// define 2D array of grids
			// int size = _mapSize / GRID_SIZE;
			// int numGrids = size*size;
			// _mapGrid = new GridBlock<GRID_SIZE>[numGrids];
		}
		~TerrainMesh()
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
			std::cout << "PRINTING VERTICES..." << std::endl;
			for (int i = 0; i < _numVertices; i++) {
				std::cout << "vertex: " << glm::to_string(_vertices[i].Position) << std::endl;
				std::cout << "normal: " << glm::to_string(_vertices[i].Normal) << std::endl;
				std::cout << "color: " << glm::to_string(_vertices[i].Color)
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

		void Render()
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