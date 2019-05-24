#pragma once

// STANDARD
#include <string>

// CUSTOM
#include "AspectRatio.h"
#include "OpenGL.h"

namespace Graphics
{
	// valid openGL function signature for input callback functions
	typedef void (*_key_callback_func)(GLFWwindow* win, int key, int scancode,
		int action, int mode);
	typedef void (*_framebuffer_size_callback) (GLFWwindow* win, int width, int height);
	typedef void (*_mouse_button_callback)(GLFWwindow* window, int button, int action,
		int mods);
	typedef void (*_mouse_callback_func)(GLFWwindow* win, double xpos, double ypos);
	typedef void (*_scroll_callback_func)(GLFWwindow* win, double xoffset, double yoffset);
	typedef void (*_window_resize_callback)(GLFWwindow* window, int width, int height);

	class ApplicationWindow
	{
	private:
		int _width, _height;
		std::string _title;

		GLFWwindow* _window;
		// bool _isMinimized; // TODO: Define use cases!

		unsigned long _glClearFlags;

		void SetupWindow()
		{
			OpenGL::InitGLFW();

			_window = glfwCreateWindow(_width, _height, _title.c_str(), NULL, NULL);
			glfwSetWindowTitle(_window, _title.c_str());

			// check if the window could be created
			if (_window == NULL)
			{
				std::cerr << "Failed to create GLFW windowed window" << std::endl;
				glfwTerminate();
				return;
			}

			// activating the window within the current thread,
			// should be done from the main thread
			glfwMakeContextCurrent(_window);

			// tell OpenGL the size of the rendering window
			// this information is retrieved from GLFW
			glfwGetFramebufferSize(_window, &_width, &_height);

			OpenGL::InitGLAD();

			// enable scissors test, so that rendering calls will only affect
			// the active viewport
			glEnable(GL_SCISSOR_TEST);
		}

	public:
		ApplicationWindow()
			: _width(500), _height(500), _title("")
		{
			_glClearFlags = GL_COLOR_BUFFER_BIT;
			_window = nullptr;
		}

		ApplicationWindow(int width, int height, std::string& title)
			: _width(width), _height(height), _title(title)
		{
			_glClearFlags = GL_COLOR_BUFFER_BIT;
			_window = nullptr;
		}

		~ApplicationWindow()
		{
			glfwDestroyWindow(_window);
			glfwTerminate();
		}

		void SetFullScreen(/* AspectRatio aspect */)
		{
			/*
			 * Something like centering the window, then figuring out how
			 * to make it as big as possible, according to screen width
			 * width and height, then fill the missing gaps with a black
			 * background fill (probably as some GUI viewport object).
			 *
			 * Alternatively, window can be filled with black background,
			 * then the viewport is just positioned correctly as the main
			 * viewport. As long as the black areas are never drawn, they
			 * simply be left alone for the remainder of the application's
			 * life time.
			 */
			OpenGL::InitGLFW();

			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			_height = 0; // ?
			// TODO: Use aspect ratio

			// TODO: What if _window is already defined??
			_window = glfwCreateWindow(mode->width, _height, _title.c_str(),
				monitor, NULL);

			if (_window == NULL)
			{
				std::cerr << "Failed to create GLFW fullscreen window" << std::endl;
				glfwTerminate();
				return;
			}

			// activating the window within the current thread,
			// should be done from the main thread
			glfwMakeContextCurrent(_window);

			// tell OpenGL the size of the rendering window
			// this information is retrieved from GLFW
			glfwGetFramebufferSize(_window, &_width, &_height);

			// parameters: bottom-left corner and the dimensions of the rendering window
			// glViewport(0, 0, _width, _height);
			// TODO: Define viewport size according to aspect ratio and screen dimensions

			OpenGL::InitGLAD();

			// enable scissors test, so that rendering calls will only affect
			// the active viewport
			glEnable(GL_SCISSOR_TEST);
		}

		void SetWindowed(AspectRatio aspect)
		{
			_width = aspect.GetWidth();
			_height = aspect.GetHeight();

			SetupWindow();

			// TODO: Resize window and all its child GUI elements

			// TODO: Define viewport GUI classes!
		}

		void CloseWindow()
		{
			glfwSetWindowShouldClose(_window, GL_TRUE);
		}

		// inline because it can be assumed to be called quite often!
		inline bool IsRunning()
		{
			return !glfwWindowShouldClose(_window);
		}

		inline int GetWidth()
		{
			return _width;
		}

		inline int GetHeight()
		{
			return _height;
		}

		std::string GetTitle()
		{
			return _title;
		}

		void SetTitle(std::string& title)
		{
			_title = title;
			glfwSetWindowTitle(_window, _title.c_str());
		}

		void SetTitle(std::string title)
		{
			_title = title;
			glfwSetWindowTitle(_window, _title.c_str());
		}

		// TODO: This is extremely insecure and should be removed!
		// TODO: This is currently being used by Cursor class.
		GLFWwindow* GetWindow()
		{
			return _window;
		}



		// --- EVENT HANDLING --- //

		// this method adds any incoming events to openGL's
		// event queue. Should be called at the start of every
		// iteration of the game loop
		void PollEvents()
		{
			glfwPollEvents();
		}

		// this method does the same as the above, except
		// here the main thread is blocked until any event is received
		void WaitEvents()
		{
			glfwWaitEvents();
		}



		// --- CALLBACK HANDLING --- //

		// use the provided function as primary key callback function
		// the function *must* not be instantiated.
		void SetKeyCallback(_key_callback_func func)
		{
			glfwSetKeyCallback(_window, func);
		}

		// glfw: whenever the window size changed (by OS or
		// user resize) this callback function executes
		void SetFramebufferSizeCallback(_framebuffer_size_callback func)
		{
			glfwSetFramebufferSizeCallback(_window, func);
		}

		// mouse callback ...
		// If you wish to be notified when the cursor moves over the window,
		// set a cursor position callback.
		void SetCursorPosCallback(_mouse_callback_func func)
		{
			glfwSetCursorPosCallback(_window, func);
		}

		// mouse callback ...
		// If you wish to be notified when a mouse button is pressed or released,
		// set a mouse button callback.
		void SetMouseButtonCallback(_mouse_button_callback func)
		{
			glfwSetMouseButtonCallback(_window, func);
		}

		// mouse callback ...
		// TODO: Add description here!
		void SetScrollCallback(_scroll_callback_func func)
		{
			glfwSetScrollCallback(_window, func);
		}

		// window resize callback
		// TODO: Add description here!
		void SetWindowResizeCallback(_window_resize_callback func)
		{
			glfwSetWindowSizeCallback(_window, func);
		}



		// --- RENDERING METHODS --- //

		// Double buffering, should be done at the end of each
		// iteration of the rendering loop
		void SwapBuffers()
		{
			glfwSwapBuffers(_window);
		}

		// clear the window to prevent artifacts from the previous
		// iteration of the rendering loop
		void ClearWindow()
		{
			// TODO: This function should attempt to clear all attached viewports,
			// through the currently bound viewport container!
			glClear(_glClearFlags);
		}

		void SetClearFlags(unsigned long flags)
		{
			_glClearFlags = flags;
		}

		void SetClearColor(GLfloat r, GLfloat g, GLfloat b)
		{
			glClearColor(r, g, b, 1.0f);
		}

		void ToggleWireframeMode()
		{
			static bool wireframe = false;

			if (!wireframe) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			wireframe = !wireframe;
		}
	};
}