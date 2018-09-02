#pragma once

#include <glad/glad.h>

class GLTexture
{
	unsigned int TexId;
	GLenum BoundTexUnit;
public:
	GLTexture(GLenum TexUnit);
	~GLTexture();
	void LoadFromDisk(char *filePath);
	unsigned int GetID();
	GLenum GetTexUnit();
};
