#pragma once
#include <GLFW/glfw3.h>

class Window
{
	static unsigned int Width;
	static unsigned int Height;
	static GLFWwindow* WindowHandle;
public:
	static GLFWwindow* Create(int w, int h);
	static void MakeCurrent();
	static bool IsActive();
	static void CheckEvents();
	static void SwapBuffers();
	static void Close();
};