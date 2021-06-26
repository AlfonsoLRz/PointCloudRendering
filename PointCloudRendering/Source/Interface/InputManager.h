#pragma once

#include "Utilities/Singleton.h"

/**
*	@file InputManager.h
*	@authors Alfonso López Ruiz (alr00048@red.ujaen.es)
*	@date 07/11/2019
*/

/**
*	@brief Receives the input events from the user, from any source. Maintains a basic state to adapt the movements taking into account previous actions.
*/
class InputManager: public Singleton<InputManager>
{
	friend class Singleton<InputManager>;

protected:
	enum MovementTypes : uint8_t
	{
		BOOM, CRANE, DOLLY, ORBIT_XZ, ORBIT_Y, PAN, TILT, TRUCK, ZOOM, NUM_MOVEMENT_TYPES
	};

	enum EventType : uint8_t
	{
		KEY_PRESS, KEY_RELEASE, MOUSE_CURSOR, MOUSE_BUTTON, MOUSE_SCROLL, RESIZE
	};

protected:
	// [Movements constraints]
	static const std::vector<float> MOVEMENT_SPEED;						//!< Movement speed for each type of movement

	static const vec2				CURSOR_POS;							//!< Default cursor position, just to be ignored
	static const float				MOVEMENT_MULTIPLIER;				//!< Multiplier for default dolly speed after each movement
	static const float				MOVEMENT_SPEED_UP;					//!< Multiplicator for general dolly speed as another key is pressed

	// [Keys constraints]
	static const int				BOOM_KEY;
	static const int				CRANE_KEY;
	static const int				DOLLY_POS_KEY;
	static const int				DOLLY_NEG_KEY;
	static const int				DOLLY_SPEED_UP_KEY;
	static const int				ITERATE_LIDAR;
	static const int				ORBIT_XZ_KEY;
	static const int				ORBIT_Y_KEY;
	static const int				PAN_KEY;
	static const int				RESET_KEY;
	static const int				SCREENSHOT_KEY;
	static const int				TILT_KEY;
	static const int				TRUCK_POS_KEY;
	static const int				TRUCK_NEG_KEY;

protected:
	// [Current state of application]
	vec2					_lastCursorPosition;							//!< Last position the cursor visited while any mouse button was clicked
	bool					_leftClickPressed, _rightClickPressed;			//!< Mouse buttons state
	int						_leftRepaints;									//!< Repaints which are still pending
	std::vector<uint16_t>	_movementsInARow;								//!< Number of consecutive movements for any type
	GLFWwindow*				_window;										//!< Window				

protected:
	/**
	*	@brief Default constructor. Initializes the state of application.
	*/
	InputManager();

	/**
	*	@brief Checks if the camera should make pan / tilt movements.
	*	@param xPos Position in x axis.
	*	@param yPos Position in y axis.
	*/
	bool checkPanTilt(const float xPos, const float yPos);

	/**
	*	@return Array with the speed of each movement. That array is assigned to the static one.
	*/
	static std::vector<float> getMovementSpeed();

	/**
	*	@brief Checks the pressed keys to modify the scene.
	*/
	bool processPressedKeyEvent(const int key, const int mods);

	/**
	*	@brief Checks the released keys to reset some scene parameters.
	*/
	bool processReleasedKeyEvent(const int key, const int mods);

public:
	/**
	*	@brief Stores GLFW window for queries. 
	*/
	void initialize(GLFWwindow* window);
	
	// [Callbacks]

	/**
	*	@brief Listens to keyboard events.
	*	@param window Window where the event has taken place.
	*	@param key Key numeric identifier.
	*	@param scancode Unique identifier of the key as a code.
	*	@param action Action taken on the key: GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE.
	*	@param mods Modifiers applied to the event: Shift, Control, etc.
	*/
	static void keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);

	/**
	*	@brief Listens to mouse button events.
	*	@param window Window where the event has taken place.
	*	@param button Identifier of the button which triggered the event.
	*	@param action Action of the user on a button: GLFW_PRESS or GLFW_RELEASE.
	*	@param mods Modifiers applied to the event: Shift, Control, etc.
	*/
	static void mouseButtonEvent(GLFWwindow* window, int button, int action, int mods);

	/**
	*	@brief Listens to mouse movement events.
	*	@param window Window where the event has taken place.
	*	@param xpos New x position of the cursor.
	*	@param ypos New y position of the cursor.
	*/
	static void mouseCursorEvent(GLFWwindow* window, double xpos, double ypos);

	/**
	*	@brief Listens to mouse scrolling events.
	*	@param window Window where the event has taken place.
	*	@param xoffset Scrolling units on the x axis.
	*	@param yoffset Scrolling units on the y axis.
	*/
	static void mouseScrollEvent(GLFWwindow* window, double xoffset, double yoffset);

	/**
	*	@brief Listens for window resizing events.
	*	@param window Window where the event has taken place.
	*	@param width Required width.
	*	@param height Required height.
	*/
	static void resizeEvent(GLFWwindow* window, int width, int height);

	/**
	*	@brief Listens for window refreshing events.
	*	@param window Window where the event has taken place.
	*/
	static void windowRefresh(GLFWwindow* window);

	// [Getter & Setter]
};

