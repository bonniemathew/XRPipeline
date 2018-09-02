#pragma once

#include "mesh.h"
#include "shader.h"
#include "gltexture.h"

class RenderPrimitive
{

protected:
	Shader *shdr;
	Mesh mesh;
	GLTexture *GeomTexture;
public:
	RenderPrimitive()
	{
		shdr = nullptr;
		GeomTexture = nullptr;
	}
	virtual ~RenderPrimitive()
	{
		// Don't delete the resources here. Already deleted from main
		//if (shdr)
		//{
		//	delete shdr;
		//	shdr = nullptr;
		//}
	}

	virtual void Build() = 0;
	virtual void Draw() = 0;
	void SetTexture(GLTexture *tex)
	{
		GeomTexture = tex;
	}
	void SetShader(Shader *shader)
	{
		shdr = shader;
	}
	void SetMesh(Mesh& msh)
	{
		mesh = msh;
	}
};
