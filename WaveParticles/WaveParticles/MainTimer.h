#pragma once

// STANDARD
#include <string>
#include <cassert>

// CUSTOM
#include "OpenGL.h"


namespace Utilities
{
	class MainTimer {
	private:
		GLfloat _lastTime;
		GLfloat _timer;
		GLfloat _updateTimeLimit;
		GLfloat _renderTimeLimit;
		GLfloat _deltaUpdateTime;
		GLfloat _deltaRenderTime;
		GLfloat _nowTime;

		int _updates, _frames;
		int _capturedUpdates, _capturedFrames;

		int _desiredMaxFPS;

	public:
		int GetCapturedUpdates() const {
			return _capturedUpdates;
		}
		int GetCapturedFrames() const {
			return _capturedFrames;
		}

		MainTimer(int ups) : MainTimer(ups, 0) {}
		MainTimer() : MainTimer(60, 60) {}

		MainTimer(int ups, int fps) {
			assert(ups >= 0);
			assert(fps >= 0);

			_updateTimeLimit = 1.0f / (GLfloat)ups;
			_renderTimeLimit = 1.0f / (GLfloat)fps;

			_desiredMaxFPS = fps;
			_lastTime = glfwGetTime();
			_deltaUpdateTime = 0.0f;
			_deltaRenderTime = 0.0f;
			_nowTime = _lastTime;

			_frames = 0;
			_updates = 0;
			_capturedUpdates = 0;
			_capturedFrames = 0;
			_timer = 0.0f;
		}

		void MeasureTime() {
			_nowTime = glfwGetTime();
			_deltaUpdateTime += (_nowTime - _lastTime) / _updateTimeLimit;
			_deltaRenderTime += (_nowTime - _lastTime) / _renderTimeLimit;
			_lastTime = _nowTime;
		}

		inline GLfloat GetDeltaTime() const {
			return _deltaUpdateTime;
		}

		bool ShouldUpdate() {
			bool ret = _deltaUpdateTime >= 1.0f;
			if (ret) {
				_updates++;
				_deltaUpdateTime--;
			}
			return ret;
		}

		bool ShouldRender() {
			if (_desiredMaxFPS < 1) {
				return true;
			}
			bool ret = _deltaRenderTime >= 1.0f;
			if (ret) {
				_frames++;
				_deltaRenderTime--;
			}
			return ret;
		}

		// The timer will reset if 1 second has passed. This information
		// can be used to update game logic in any way desirable.
		bool ShouldReset() {
			bool ret = glfwGetTime() - _timer > 1.0f;
			if (ret) {
				_timer += 1.0f;
				_capturedUpdates = _updates;
				_capturedFrames = _frames;
				_updates = 0;
				_frames = 0;
			}
			return ret;
		}

		std::string GetTimeTitle()
		{
			std::string title;
			title += "UPS " + std::to_string(_capturedUpdates);
			title += ", FPS " + std::to_string(_capturedFrames);
			return title;
		}
	};
}