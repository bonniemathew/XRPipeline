#include <iostream>
#include "glframebuffer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

GLFramebuffer::GLFramebuffer(int W, int H)
{
	FBId = 0;
	ColorBufferId = 0;
	RenderBufferId = 0;
	Width = W;
	Height = H;
}

GLFramebuffer::~GLFramebuffer()
{
	glDeleteFramebuffers(1, &FBId);
}

void GLFramebuffer::Generate()
{
	glGenFramebuffers(1, &FBId);
	glBindFramebuffer(GL_FRAMEBUFFER, FBId);
	// create a color attachment texture
	glGenTextures(1, &ColorBufferId);
	glBindTexture(GL_TEXTURE_2D, ColorBufferId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorBufferId, 0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	glGenRenderbuffers(1, &RenderBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RenderBufferId); // now actually attach it
																									   // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

	// unbind the fb
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void GLFramebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBId); // todo: need to save the current framebuffer and set it in the unbind
}
void GLFramebuffer::UnBind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // todo: need to restore the prev binding rather than blindly binding to 0
}
unsigned int GLFramebuffer::GetColorBufferId()
{
	return ColorBufferId;
}