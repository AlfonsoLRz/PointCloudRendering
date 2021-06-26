#include "stdafx.h"
#include "Window.h"

#include "Graphics/Application/Renderer.h"
#include "Interface/GUI.h"

/// [Protected methods]

Window::Window(): Singleton(), _size(1, 1), _window(nullptr), _windowState(NOT_LOADED)
{
}

/// [Public methods]

void Window::changedSize(const uint16_t width, const uint16_t height)
{
	_size = ivec2(width, height);
}

bool Window::load(const std::string& title, const uint16_t width, const uint16_t height, const uint8_t openGL4Version)
{
	_size = ivec2(width, height);

	if (glfwInit() != GLFW_TRUE)
	{
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);										// Antialiasing
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);			// OpenGL Core Profile 4.5
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glm::clamp((int) openGL4Version, 1, 6));

	_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

	if (_window == nullptr) {												// Window initialization could fail in case the computer doesn't meet the required specs
		glfwTerminate();

		return false;
	}

	glfwMakeContextCurrent(_window);										// From now on the window uses the context with the properties cited previously
	glfwSwapInterval(1);

	glewExperimental = true;			// !! GLFW -> GLEW; order matters
	if (glewInit() != GLEW_OK) {
		glfwTerminate();	

		return false;
	}

	std::cout << glGetString(GL_RENDERER) << std::endl;						// 3D context properties
	std::cout << glGetString(GL_VENDOR) << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	// Callbacks to main events
	glfwSetWindowRefreshCallback(_window, InputManager::windowRefresh);
	glfwSetFramebufferSizeCallback(_window, InputManager::resizeEvent);
	glfwSetKeyCallback(_window, InputManager::keyEvent);
	glfwSetMouseButtonCallback(_window, InputManager::mouseButtonEvent);
	glfwSetCursorPosCallback(_window, InputManager::mouseCursorEvent);
	glfwSetScrollCallback(_window, InputManager::mouseScrollEvent);

	// OpenGL environment
	Renderer::getInstance()->prepareOpenGL(width, height);	
	GUI::getInstance()->initialize(_window, openGL4Version);
	InputManager::getInstance()->initialize(_window);

	_windowState = SUCCESSFUL_LOAD;	

	return true;
}

void Window::startRenderingCycle()
{
	if (_windowState != SUCCESSFUL_LOAD) return;

	InputManager* inputManager = InputManager::getInstance();

	while (!glfwWindowShouldClose(_window)) {			// Application event cycle. We don't get out of here until window is closed by the user
		glfwPollEvents();								// Obtains and organizes any event
		InputManager::getInstance()->windowRefresh(_window);
	}

	glfwDestroyWindow(_window);							// Free GLFW resources
	glfwTerminate();
}
