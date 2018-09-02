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
#include "gltexture.h"

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

// Eye buffer enums
typedef enum
{
	EYE_LEFT =0,
	EYE_RIGHT
}Eye;

// GLFW callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
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

void RenderScene(Eye e, Shader &SceneShader, GLFramebuffer &FB_L, GLFramebuffer &FB_R, std::list<RenderPrimitive*> &RenderList)
{
	float FOVy = 90;
	float CamAspect = 1.0f;
	float Near = 0.1f;
	float Far = 1000.0f;
	float HalfIPD = 0.064f*0.5f;

	// activate shader
	SceneShader.use();
	 
	if (e == EYE_LEFT) 		// render left
	{
		// bind the framebuffer to render to the texture
		FB_L.Bind();
		glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
		glViewport(15, 15, SCR_WIDTH / 2 - 30, SCR_HEIGHT - 30); // to have a black border around the eye buffer


		glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), CamAspect, Near, Far);
		SceneShader.setMat4("projection", projection);

		// view transformation for right eye
		glm::mat4 view = camera.GetViewMatrix();
		view = glm::translate(view, glm::vec3(-HalfIPD, 0, 0));
		SceneShader.setMat4("view", view);


	}
	else 		// render right
	{

		FB_R.Bind();
		glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(15, 15, SCR_WIDTH / 2 - 30, SCR_HEIGHT - 30); // to have a black border around the eye buffer



		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), CamAspect, Near, Far);
		SceneShader.setMat4("projection", projection);

		// view transformation for right eye
		glm::mat4 view = camera.GetViewMatrix();
		view = glm::translate(view, glm::vec3(HalfIPD, 0, 0));
		SceneShader.setMat4("view", view);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// todo: shader and model matrix should be part of the model itself. Move it to RenderPrimitive
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, glm::vec3(0, -2.5f, 0));
	SceneShader.setMat4("model", model);

	for (list<RenderPrimitive*>::iterator iter = RenderList.begin(); iter != RenderList.end(); iter++)
	{
		if (*iter)
		{
			(*iter)->Draw();
		}
	}

	// Now unbind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int main()
{
	// List of Scene primitives (in this case, cylinder)
	std::list<RenderPrimitive*> ScenePrimitivesList;
	// List of primitives for the pipeline
	std::list<RenderPrimitive*> PipelinePrimitivesList;

	GLFWwindow* Wnd = Window::Create(SCR_WIDTH, SCR_HEIGHT);
	Window::MakeCurrent();

	// Set the call backs for events
	glfwSetFramebufferSizeCallback(Wnd, framebuffer_size_callback); // callback for window resizing
	glfwSetScrollCallback(Wnd, scroll_callback); // call back for scroll events
	glfwSetInputMode(Wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(Wnd, GLFW_STICKY_KEYS, 0);

	// Init GL
	if (!InitGL())
		return -1;

	// Create cylinder - scene primitive
	float Height = 5;
	float Width = 3;
	int CrossSectionCount = 30;
	RenderPrimitive *Cyl = new Cylinder(Height, Width, CrossSectionCount);
	Cyl->Build();
	ScenePrimitivesList.push_back(Cyl);

	// Create quad for rendering the eye buffer
	RenderPrimitive *EyeBufferQuad = new Quad();
	EyeBufferQuad->Build();
	PipelinePrimitivesList.push_back(EyeBufferQuad);

	glEnable(GL_DEPTH_TEST);

	// build and compile shader programs
	Shader SceneShader("shaders/v_shader.vs", "shaders/f_shader.fs");
	Shader DistShader("shaders/v_screen_shader.vs", "shaders/f_dist_shader.fs");
	Shader NoDistShader("shaders/v_screen_shader.vs", "shaders/f_screen_shader.fs");

	// Create FB for eye buffers
	GLFramebuffer FB_L(SCR_WIDTH*0.5f, SCR_HEIGHT);
	FB_L.Generate();

	GLFramebuffer FB_R(SCR_WIDTH*0.5f, SCR_HEIGHT);
	FB_R.Generate();

	
	// load the eye buffer textures
	// texture for the left cylinder
	GLTexture TexL(GL_TEXTURE0);
	TexL.LoadFromDisk("resources/left.jpg");
	// texture for the right cylinder
	GLTexture TexR(GL_TEXTURE0);
	TexR.LoadFromDisk("resources/right.jpg");


	SceneShader.use();
	SceneShader.setInt("texture1", 0);

	DistShader.use();
	DistShader.setInt("screenTexture", 3);

	NoDistShader.use();
	DistShader.setInt("screenTexture", 3);


	// Render Loop
	while (Window::IsActive())
	{

		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;



		// Handle Inputs
		processInput(Wnd);



		// Render Scene - LEFT and RIGHT eye
		Cyl->SetTexture(&TexL);
		RenderScene(EYE_LEFT, SceneShader, FB_L, FB_R, ScenePrimitivesList); // todo: each object should have its own shader (material)
		
		Cyl->SetTexture(&TexR);
		RenderScene(EYE_RIGHT, SceneShader, FB_L, FB_R, ScenePrimitivesList); // rather than single shader for entire scene




		// Final render
		if (bApplyDistortion) // can be toggled based on the keypress ('V')
		{
			DistShader.use();
		}
		else
		{
			NoDistShader.use();
		}

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		// Final render to the display - RIGHT
		glViewport(SCR_WIDTH / 2, 0, SCR_WIDTH / 2, SCR_HEIGHT);
		glActiveTexture(GL_TEXTURE3); // todo: move the Shader and texture resources also to the RenderPrimitive
		glBindTexture(GL_TEXTURE_2D, FB_R.GetColorBufferId());
		EyeBufferQuad->Draw();

		// Final render to the display - LEFT
		glViewport(0, 0, SCR_WIDTH / 2, SCR_HEIGHT);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, FB_L.GetColorBufferId());
		EyeBufferQuad->Draw();

	
		glBindVertexArray(0);


		// Do the swap buffers
		Window::SwapBuffers();
		Window::CheckEvents();

	}

	// Clear the Scene Primitives List 
	for (list<RenderPrimitive*>::iterator iter = ScenePrimitivesList.begin(); iter != ScenePrimitivesList.end(); iter++)
	{
		if (*iter)
		{
			delete *iter;
		}
	}
	ScenePrimitivesList.clear();

	// Clear the PipelinePrimitivesList 
	for (list<RenderPrimitive*>::iterator iter = PipelinePrimitivesList.begin(); iter != PipelinePrimitivesList.end(); iter++)
	{
		if (*iter)
		{
			delete *iter;
		}
	}
	PipelinePrimitivesList.clear();
	

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
			// Toggle distortion
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



// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}