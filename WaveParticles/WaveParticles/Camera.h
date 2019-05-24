#pragma once

// CUSTOM
#include "OpenGL.h"


namespace Graphics
{
	class BasicRTSCamera
	{
	private:
	protected:
		GLfloat _fov, _yaw, _pitch;
		const GLfloat MIN_FOV = 44.3f;
		const GLfloat MAX_FOV = 45.0f;

		// clipping planes
		GLfloat _near, _far;

		// camera's position vectors
		glm::vec3 _position, _front, _up;

		// transform matrices
		glm::mat4 _view, _projection;
		glm::mat4 _invView, _invProjection;

		// window properties
		glm::ivec2 _windowSize;
		GLfloat _aspectRatio;

		// mouse properties
		GLfloat _lastX, _lastY;

		void CalculateAspectRatio()
		{
			_aspectRatio = (GLfloat)_windowSize.x / (GLfloat)_windowSize.y;
		}

	public:
		BasicRTSCamera(int width, int height, glm::vec3 pos,
			glm::vec3 front, glm::vec3 up)
		{
			_near = 0.1f;
			_far = 200.0f;

			_windowSize.x = width;
			_windowSize.y = height;
			CalculateAspectRatio();

			_fov = 45.0f;
			_yaw = -90.f;
			_pitch = 0.0f;

			_lastX = (GLfloat)_windowSize.x / 2.0f;
			_lastY = (GLfloat)_windowSize.y / 2.0f;

			_position = pos;
			_front = glm::normalize(front);
			_up = glm::normalize(up);

			CalculateViewProjection();
		}

		~BasicRTSCamera() { }

		void MoveNorth(GLfloat cameraSpeed)
		{
			_position.x += (_front.x + _up.x) * cameraSpeed;
			_position.y += (_front.y + _up.y) * cameraSpeed;
		}
		void MoveSouth(GLfloat cameraSpeed)
		{
			_position.x -= (_front.x + _up.x) * cameraSpeed;
			_position.y -= (_front.y + _up.y) * cameraSpeed;
		}
		void MoveWest(GLfloat cameraSpeed)
		{
			_position -= glm::normalize(glm::cross(_front, _up)) * cameraSpeed;
		}
		void MoveEast(GLfloat cameraSpeed)
		{
			_position += glm::normalize(glm::cross(_front, _up)) * cameraSpeed;
		}

		void MoveUp(GLfloat cameraSpeed)
		{
			_position.z += cameraSpeed;
		}
		void MoveDown(GLfloat cameraSpeed)
		{
			_position.z -= cameraSpeed;
		}

		void RotateCamera(GLfloat angle)
		{
			// camera ray intersection point with scene (where z = 0)
			GLfloat t = -_position.z / _front.z;
			glm::vec3 scene_intersection = _position + _front * t;

			// vector from camera point to intersection
			glm::vec3 dist = scene_intersection - _position;
			// printf("dist: (%g,%g,%g)\n", dist.x, dist.y, dist.z);

			// rotate front and up vectors
			_front = glm::normalize(glm::rotateZ(_front, angle));
			_up = glm::normalize(glm::rotateZ(_up, angle));

			// rotate a 3d vector around an axis
			_position = scene_intersection + glm::rotateZ(-dist, angle);
		}

		void RotateLeft(GLfloat cameraSpeed)
		{
			GLfloat angle = glm::pi<GLfloat>() * 0.1f * cameraSpeed;
			RotateCamera(angle);
		}

		void RotateRight(GLfloat cameraSpeed)
		{
			GLfloat angle = glm::pi<GLfloat>() * -0.1f * cameraSpeed;
			RotateCamera(angle);
		}

		// update the camera's view and projection matrices
		// according to any changes to the position vectors
		//
		// This function should be called at each iteration
		// of the game loop.
		void CalculateViewProjection()
		{
			_view = glm::lookAt(_position, _position + _front, _up);
			_projection = glm::perspective(_fov, _aspectRatio,
				_near, _far);
		}
		void CalculateInverseViewProjection()
		{
			_invView = glm::inverse(_view);
			_invProjection = glm::inverse(_projection);
		}

		void MouseScrollCallback(double xoffset, double yoffset)
		{
			if (_fov >= MIN_FOV && _fov <= MAX_FOV) { _fov -= yoffset * 0.1f; }
			if (_fov <= MIN_FOV) { _fov = MIN_FOV; }
			if (_fov >= MAX_FOV) { _fov = MAX_FOV; }
		}

		// TODO: Should not all these functions be marked as const and return const& ?!

		glm::vec3 GetViewDir()
		{
			return _front;
		}

		glm::vec3 GetPosition()
		{
			return _position;
		}

		const glm::vec3* GetPosition() const
		{
			return const_cast<glm::vec3*>(&_position);
		}

		const glm::mat4* GetViewMatrix() const
		{
			return const_cast<glm::mat4*>(&_view);
		}
		const glm::mat4* GetProjectionMatrix() const
		{
			return const_cast<glm::mat4*>(&_projection);
		}

		glm::mat4* GetInverseViewMatrix()
		{
			return &_invView;
		}
		glm::mat4* GetInverseProjectionMatrix()
		{
			return &_invProjection;
		}
	};
}