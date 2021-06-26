#pragma once

#include "Geometry/2D/Vector2.h"
#include "Interface/InputManager.h"
#include "Utilities/Singleton.h"

/**
*	@file Window.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/09/2019
*/

/**
*	@brief Main window of the application, provided by GLFW.
*/
class Window: public Singleton<Window>
{
	friend class Singleton<Window>;

protected:
	enum Codes { NOT_LOADED, SUCCESSFUL_LOAD, UNSUCCESSFUL_LOAD};

	ivec2		_size;									//!< Dimensions of window
	GLFWwindow* _window;								//!< Loaded GLFW window, if any
	uint8_t		_windowState;							//!< Code of window state as a reference for event cycle petitions

protected:
	/**
	*	@brief Private constructor as this class implement the Singleton pattern.
	*/
	Window();

public:
	/**
	*	@brief Window resized event.
	*	@param width New width of window.
	*	@param height New height of window.
	*/
	void changedSize(const uint16_t width, const uint16_t height);

	/**
	*	@return Size of window.	First coordinate: width, second coordinate: height.
	*/
	ivec2 getSize() { return _size; }

	/**
	*	@brief Initializes the window resources and the GLFW context. Note: it is not needed to ensure this method is called just once
	*		   as the application enters a loop in here.
	*	@param title Title of the window.
	*	@param width Initial width of the window.
	*	@param height Initial height of the window.
	*	@param openGL4Version Version of OpenGL 4 which is required for this application [0, 6].
	*	@return Success of initialization.
	*/
	bool load(const std::string& title, const uint16_t width, const uint16_t height, const uint8_t openGL4Version = 6);

	/**
	*	@brief Start the event and rendering cycle if possible (window must have been successfully loaded).
	*/
	void startRenderingCycle();
};

