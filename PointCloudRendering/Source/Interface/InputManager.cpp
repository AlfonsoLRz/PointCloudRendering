#include "stdafx.h"
#include "InputManager.h"

#include "Geometry/General/BasicOperations.h"
#include "Graphics/Application/SSAOScene.h"
#include "Graphics/Application/Renderer.h"
#include "Graphics/Core/Camera.h"
#include "Interface/GUI.h"
#include "Interface/Window.h"

// [Static members initialization]

const std::vector<float> InputManager::MOVEMENT_SPEED = getMovementSpeed();

const vec2	InputManager::CURSOR_POS = vec2(-1.0f, -1.0f);					//!< Default cursor position, just to be ignored
const float InputManager::MOVEMENT_MULTIPLIER = 0.1f;					
const float InputManager::MOVEMENT_SPEED_UP = 1.0f;						

const int InputManager::BOOM_KEY = GLFW_KEY_UP;
const int InputManager::CRANE_KEY = GLFW_KEY_DOWN;
const int InputManager::DOLLY_POS_KEY = GLFW_KEY_W;
const int InputManager::DOLLY_NEG_KEY = GLFW_KEY_S;
const int InputManager::DOLLY_SPEED_UP_KEY = GLFW_MOD_SHIFT;
const int InputManager::ITERATE_LIDAR = GLFW_KEY_I;
const int InputManager::ORBIT_XZ_KEY = GLFW_KEY_Y;
const int InputManager::ORBIT_Y_KEY = GLFW_KEY_X;
const int InputManager::PAN_KEY = GLFW_KEY_P;
const int InputManager::RESET_KEY = GLFW_KEY_R;
const int InputManager::SCREENSHOT_KEY = GLFW_KEY_K;
const int InputManager::TILT_KEY = GLFW_KEY_T;
const int InputManager::TRUCK_POS_KEY = GLFW_KEY_D;
const int InputManager::TRUCK_NEG_KEY = GLFW_KEY_A;

/// [Protected methods]

InputManager::InputManager() :
	_lastCursorPosition(CURSOR_POS), _leftClickPressed(false), _leftRepaints(1), _rightClickPressed(false),
	_movementsInARow(NUM_MOVEMENT_TYPES), _window(nullptr)
{
}

bool InputManager::checkPanTilt(const float xPos, const float yPos)
{
	Camera* camera = Renderer::getInstance()->getActiveCamera();

	_leftClickPressed &= glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	_rightClickPressed &= glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
	
	if (_leftClickPressed || _rightClickPressed)
	{	
		if (_lastCursorPosition.x >= 0.0f)						// Any displacement?
		{
			if (!BasicOperations::equal(xPos, _lastCursorPosition.x))
			{
				camera->pan(-MOVEMENT_SPEED[PAN] * (xPos - _lastCursorPosition.x));
			}

			if (!BasicOperations::equal(yPos, _lastCursorPosition.y))
			{
				camera->tilt(-MOVEMENT_SPEED[TILT] * (yPos - _lastCursorPosition.y));
			}
		}

		_lastCursorPosition = vec2(xPos, yPos);

		return true;
	}

	return false;
}

std::vector<float> InputManager::getMovementSpeed()
{
	std::vector<float> movSpeed(NUM_MOVEMENT_TYPES);

	movSpeed[BOOM]		= 0.1f;
	movSpeed[CRANE]		= 0.1f;
	movSpeed[DOLLY]		= 0.08f;
	movSpeed[ORBIT_XZ]	= 0.05f;
	movSpeed[ORBIT_Y]	= 0.03f;
	movSpeed[PAN]		= 0.002f;
	movSpeed[TILT]		= 0.002f;
	movSpeed[TRUCK]		= 0.01f;
	movSpeed[ZOOM]		= 0.008f;

	return movSpeed;
}

bool InputManager::processPressedKeyEvent(const int key, const int mods)
{
	bool repaint					= true;
	Renderer* renderer				= Renderer::getInstance();
	Camera* camera					= renderer->getActiveCamera();
	RenderingParameters* rendParams = renderer->getRenderingParameters();
	InputManager* inputManager		= InputManager::getInstance();

	switch (key)
	{
	case InputManager::RESET_KEY:
		camera->reset();

		break;

	case InputManager::ORBIT_XZ_KEY:
		if (mods == GLFW_MOD_CONTROL)
		{
			camera->orbitXZ(MOVEMENT_SPEED[ORBIT_XZ]);
		}
		else
		{
			camera->orbitXZ(-MOVEMENT_SPEED[ORBIT_XZ]);
		}

		break;

	case InputManager::ORBIT_Y_KEY:
		if (mods == GLFW_MOD_CONTROL)
		{
			camera->orbitY(-MOVEMENT_SPEED[ORBIT_Y]);
		}
		else
		{
			camera->orbitY(MOVEMENT_SPEED[ORBIT_Y]);
		}

		break;

	case InputManager::DOLLY_POS_KEY:
		if (_rightClickPressed)
		{
			camera->dolly(MOVEMENT_SPEED[DOLLY] + _movementsInARow[DOLLY] * MOVEMENT_SPEED[DOLLY] * MOVEMENT_MULTIPLIER);
			++_movementsInARow[DOLLY];
		}

		break;

	case InputManager::DOLLY_NEG_KEY:
		if (_rightClickPressed)
		{
			camera->dolly(-(MOVEMENT_SPEED[DOLLY] + _movementsInARow[DOLLY] * MOVEMENT_SPEED[DOLLY] * MOVEMENT_MULTIPLIER));
			++_movementsInARow[DOLLY];
		}

		break;

	case InputManager::TRUCK_POS_KEY:
		if (_rightClickPressed)
		{
			camera->truck(MOVEMENT_SPEED[TRUCK] + _movementsInARow[TRUCK] * MOVEMENT_SPEED[TRUCK] * MOVEMENT_MULTIPLIER);
			++_movementsInARow[TRUCK];
		}

		break;

	case InputManager::TRUCK_NEG_KEY:
		if (_rightClickPressed)
		{
			camera->truck(-(MOVEMENT_SPEED[TRUCK] + _movementsInARow[TRUCK] * MOVEMENT_SPEED[TRUCK] * MOVEMENT_MULTIPLIER));
			++_movementsInARow[TRUCK];
		}

		break;

	case InputManager::BOOM_KEY:
		camera->boom(MOVEMENT_SPEED[BOOM]);

		break;

	case InputManager::CRANE_KEY:
		camera->crane(MOVEMENT_SPEED[BOOM]);

		break;
	default:
		repaint = false;
	};

	{
		switch(key)
		{
		case InputManager::SCREENSHOT_KEY:
			renderer->getScreenshot(rendParams->_screenshotFilenameBuffer);

			break;
			
		default:
			repaint = false;
		};
	}

	return repaint;
}

bool InputManager::processReleasedKeyEvent(const int key, const int mods)
{
	bool repaint = false;
	InputManager* inputManager = InputManager::getInstance();

	if (key == DOLLY_POS_KEY || key == DOLLY_NEG_KEY)
	{
		_movementsInARow[DOLLY] = 0;
		repaint = true;
	}

	if (key == TRUCK_POS_KEY || key == TRUCK_NEG_KEY)
	{
		_movementsInARow[TRUCK] = 0;
		repaint = true;
	}

	return repaint;
}

/// [Public methods]

void InputManager::initialize(GLFWwindow* window)
{
	_window = window;
}

void InputManager::keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	bool repaint = false;
	InputManager* inputManager = InputManager::getInstance();

	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		repaint = inputManager->processPressedKeyEvent(key, mods);
	}
	else
	{
		repaint = inputManager->processReleasedKeyEvent(key, mods);
	}

	if (repaint)
	{
		windowRefresh(window);
	}
}

void InputManager::mouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
{
	InputManager* inputManager = InputManager::getInstance();

	if (GUI::getInstance()->isMouseActive()) return;

	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		inputManager->_leftClickPressed = action == GLFW_PRESS;
		inputManager->_lastCursorPosition = CURSOR_POS;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		inputManager->_rightClickPressed = action == GLFW_PRESS;
		inputManager->_lastCursorPosition = CURSOR_POS;
	}
}

void InputManager::mouseCursorEvent(GLFWwindow* window, double xpos, double ypos)
{
	InputManager* inputManager = InputManager::getInstance();

	if (!GUI::getInstance()->isMouseActive() && inputManager->checkPanTilt(xpos, ypos))
	{
		windowRefresh(window);
	}
}

void InputManager::mouseScrollEvent(GLFWwindow* window, double xoffset, double yoffset)
{
	InputManager* inputManager = InputManager::getInstance();
	Camera* camera = Renderer::getInstance()->getActiveCamera();

	if (!GUI::getInstance()->isMouseActive())
	{
		camera->zoom(yoffset * MOVEMENT_SPEED[ZOOM]);
		windowRefresh(window);
	}
}

void InputManager::resizeEvent(GLFWwindow* window, int width, int height)
{
	InputManager* inputManager = InputManager::getInstance();
	Window::getInstance()->changedSize(width, height);
	Renderer::getInstance()->resize(width, height);
}

void InputManager::windowRefresh(GLFWwindow* window)
{
	Renderer::getInstance()->render();
	GUI::getInstance()->render();

	glfwSwapBuffers(window);
}
