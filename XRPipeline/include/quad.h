#pragma once

#include "renderprimitive.h"

class Quad : public RenderPrimitive
{
	unsigned int QuadVAO, QuadVBO;
public:
	Quad();
	~Quad();

	void Build() override;
	void Draw() override;
};