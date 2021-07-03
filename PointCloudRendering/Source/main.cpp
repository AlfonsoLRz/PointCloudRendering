#include "stdafx.h"
#include "Interface/Window.h"
#include <windows.h>						// DWORD is undefined otherwise

// Laptop support. Use NVIDIA graphic card instead of Intel
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


int main(int argc, char *argv[])
{
	srand(time(nullptr));
	
	std::cout << "__ Starting Point Cloud Renderer __" << std::endl;

	const std::string title = "Point Cloud Renderer";
	const uint16_t width = 1050, height = 650;
	const auto window = Window::getInstance();
	
	{
		if (const bool success = window->load(title, width, height))
		{
			window->startRenderingCycle();

			std::cout << "__ Point Cloud Renderer __" << std::endl;
		}
		else
		{
			std::cout << "__ Failed to initialize GLFW __" << std::endl;
		}
	}

	system("pause");

	return 0;
}
