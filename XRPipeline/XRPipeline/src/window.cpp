#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "window.h"
#include <iostream>

using namespace std;

GLFWwindow* Window::Create(int W, int H)
{
	Width = W;
	Height = H;

	WindowHandle = nullptr;

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	WindowHandle = glfwCreateWindow(Width, Height, "CylindricalMapVR", NULL, NULL);
	if (WindowHandle == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 0;
	}

	return WindowHandle;
}

bool Window::IsActive()
{
	return !glfwWindowShouldClose(WindowHandle);
}

void Window::MakeCurrent()
{
	glfwMakeContextCurrent(WindowHandle);
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(WindowHandle);
}

void Window::CheckEvents()
{
	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwPollEvents();
}



void Window::Close()
{
	glfwTerminate();
}
