#include <iostream>
#include <list>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
 
#include <shader.h>
#include <camera.h>
#include "Cylinder.h"
#include "glframebuffer.h"
#include "window.h"
#include "quad.h"

using namespace std;


// Window to Render
unsigned int Window::Width = 0;
unsigned int Window::Height = 0;
GLFWwindow* Window::WindowHandle = nullptr;

// Screen Settings
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;

// Camera Settings
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Time tracking
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float KBDistEventTime = 0.0f;
float KBEventWaitTime = 0.3f;

// Distortion Toggle
bool bApplyDistortion = true;


// GLFW callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

bool InitGL()
{
	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	return true;
}

void RenderScene()
{

}

// todo1: Seperate the scene rendering to RenderScene(left/right);
// todo2: move the textures to texture class
// todo3: Add button to toggle distortion


int main()
{

	std::list<RenderPrimitive*> RenderList;

	GLFWwindow* Wnd = Window::Create(SCR_WIDTH, SCR_HEIGHT);
	Window::MakeCurrent();
	// Set the call backs for events
	glfwSetFramebufferSizeCallback(Wnd, framebuffer_size_callback); // callback for window resizing
	glfwSetCursorPosCallback(Wnd, mouse_callback); // callback for mouse events
	glfwSetScrollCallback(Wnd, scroll_callback); // call back for scroll events
	glfwSetInputMode(Wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(Wnd, GLFW_STICKY_KEYS, 0);

	// Init GL
	if (!InitGL())
		return -1;

	// Create cylinder and quads
	float Height = 5;
	float Width = 3;
	int CrossSectionCount = 30;
	RenderPrimitive *Cyl = new Cylinder(Height, Width, CrossSectionCount);
	Cyl->Build();
	RenderList.push_back(Cyl);

	RenderPrimitive *EyeBufferQuad = new Quad();
	EyeBufferQuad->Build();
	RenderList.push_back(EyeBufferQuad);

	glEnable(GL_DEPTH_TEST);

	// build and compile our shader programs
	Shader SceneShader("shaders/v_shader.vs", "shaders/f_shader.fs");
	Shader DistShader("shaders/v_screen_shader.vs", "shaders/f_dist_shader.fs");
	Shader NoDistShader("shaders/v_screen_shader.vs", "shaders/f_screen_shader.fs");

	// Create FB for eye buffers
	GLFramebuffer FB_L(SCR_WIDTH*0.5f, SCR_HEIGHT);
	FB_L.Generate();

	GLFramebuffer FB_R(SCR_WIDTH*0.5f, SCR_HEIGHT);
	FB_R.Generate();

	
	// load and create a texture 
	unsigned int textureLeft, textureRight;
	// texture 1
	// ---------
	glGenTextures(1, &textureLeft);
	glBindTexture(GL_TEXTURE_2D, textureLeft);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load("resources/left.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	// texture 2
	// ---------
	glGenTextures(1, &textureRight);
	glBindTexture(GL_TEXTURE_2D, textureRight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	data = stbi_load("resources/right.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	SceneShader.use();
	SceneShader.setInt("texture1", 0);

	DistShader.use();
	DistShader.setInt("screenTexture", 3);

	bool bRenderToFB = true;

	// render loop
	while (Window::IsActive())
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(Wnd);

		// bind the framebuffer to render to the texture
		FB_L.Bind();
		glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)


		// render left
		//---------------------------------
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(15, 30, SCR_WIDTH / 2 - 30, SCR_HEIGHT - 60);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureLeft);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureRight);

		// activate shader
		SceneShader.use();

		float FOVy = 90;
		float CamAspect = 1.0f;
		float Near = 0.1f;
		float Far = 1000.0f;
		float HalfIPD = 0.064f*0.5f;
		float FrustumShift = (HalfIPD)  * Near / 10.0f;

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projectionL = glm::perspective(glm::radians(camera.Fov), CamAspect, Near, Far);
		SceneShader.setMat4("projection", projectionL);

		// camera/view transformation
		glm::mat4 viewL = camera.GetViewMatrix();
		viewL = glm::translate(viewL, glm::vec3(-HalfIPD, 0, 0));
		SceneShader.setMat4("view", viewL);

		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, -2.5f, 0));
		SceneShader.setMat4("model", model);

		Cyl->Draw();




		// render right
		// ----------------------------
		FB_R.Bind();
		glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(15, 30, SCR_WIDTH / 2 - 30, SCR_HEIGHT - 60);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureRight);


		// activate shader
		SceneShader.use();

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projectionR = glm::perspective(glm::radians(camera.Fov), CamAspect, Near, Far);
		SceneShader.setMat4("projection", projectionR);

		// camera/view transformation
		glm::mat4 viewR = camera.GetViewMatrix();
		viewR = glm::translate(viewR, glm::vec3(HalfIPD, 0, 0));
		SceneShader.setMat4("view", viewR);

		// calculate the model matrix for each object and pass it to shader before drawing
		SceneShader.setMat4("model", model);


		// render Cylinder
		Cyl->Draw();


		
		// Now unbind the framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

			
		// Render to the Right Viewport
		if (bApplyDistortion)
		{
			DistShader.use();
		}
		else {
			NoDistShader.use();
		}
		glViewport(SCR_WIDTH / 2, 0, SCR_WIDTH / 2, SCR_HEIGHT);
		glActiveTexture(GL_TEXTURE3); // todo: move the Shader and texture resources also to the RenderPrimitive
		glBindTexture(GL_TEXTURE_2D, FB_R.GetColorBufferId());
		EyeBufferQuad->Draw();

		// Render to the Left Viewport
		glViewport(0, 0, SCR_WIDTH / 2, SCR_HEIGHT);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, FB_L.GetColorBufferId());
		EyeBufferQuad->Draw();

	
		glBindVertexArray(0);

		Window::SwapBuffers();
		Window::CheckEvents();

	}

	// Clear the RenderList
	for (list<RenderPrimitive*>::iterator iter = RenderList.begin(); iter != RenderList.end(); iter++)
	{
		if (*iter)
		{
			delete *iter;
		}
	}
	RenderList.clear();

	return 0;
}

// process all input
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && abs(lastFrame - KBDistEventTime) >= KBEventWaitTime)
	{
			bApplyDistortion ^= 1;
			KBDistEventTime = lastFrame;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}