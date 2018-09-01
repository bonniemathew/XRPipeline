#pragma once

class GLFramebuffer // can inherit from a generic Framebuffer class and derived class can be platform specific
{
	unsigned int FBId;
	unsigned int ColorBufferId;
	unsigned int RenderBufferId;
	int Width;
	int Height;
public:
	GLFramebuffer(int W, int H);
	~GLFramebuffer();

	void Generate();
	void Bind();
	void UnBind();
	unsigned int GetColorBufferId();
};
