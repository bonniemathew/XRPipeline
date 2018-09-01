#pragma once

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderprimitive.h"

class Cylinder : public RenderPrimitive
{
	unsigned int VBO[2], VAO, VIB;
	float Height;
	float Radius;
	int SubDivCount;
	bool bIsDoubleSided;
	bool bSmoothNormals;
public:
	Cylinder(float Height, float Radius, int CrossSectionCount);
	Cylinder();
	~Cylinder();
	void Build() override;
	void Draw() override;
	
};