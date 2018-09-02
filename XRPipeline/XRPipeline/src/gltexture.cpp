#include "gltexture.h"
#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>

GLTexture::GLTexture(GLenum TexUnit)
{
	BoundTexUnit = TexUnit;
}
GLTexture::~GLTexture()
{
	glDeleteTextures(1, &TexId);
}
void GLTexture::LoadFromDisk(char *filePath)
{
	// Left Eye Buffer texture from the disk
	glGenTextures(1, &TexId);
	glBindTexture(GL_TEXTURE_2D, TexId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load(filePath, &width, &height, &nrChannels, 0);
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
}
unsigned int GLTexture::GetID()
{
	return TexId;
}
GLenum GLTexture::GetTexUnit()
{
	return BoundTexUnit;
}
