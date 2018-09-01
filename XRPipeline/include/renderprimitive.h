#pragma once

#include "mesh.h"
#include "shader.h"

class RenderPrimitive
{

protected:
	Shader *shdr;
	Mesh mesh;
public:
	RenderPrimitive()
	{
		shdr = nullptr;
	}
	virtual ~RenderPrimitive()
	{
		if (shdr)
		{
			delete shdr;
			shdr = nullptr;
		}
	}

	virtual void Build() = 0;
	virtual void Draw() = 0;
	void SetShader(Shader *shader)
	{
		shdr = shader;
	}
	void SetMesh(Mesh& msh)
	{
		mesh = msh;
	}
};
