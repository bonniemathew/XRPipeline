#include "quad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderprimitive.h"

Quad::Quad()
	: RenderPrimitive()
{
	QuadVAO = 0;
	QuadVBO = 0;
}

Quad::~Quad()
{
	glDeleteVertexArrays(1, &QuadVAO);
	glDeleteBuffers(1, &QuadVBO);
}

void Quad::Build()
{
		//  For drawing the final screen space quad
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	mesh.vertices.push_back(glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f));
	mesh.vertices.push_back(glm::vec4(-1.0f, -1.0f, 0.0f, 0.0f));
	mesh.vertices.push_back(glm::vec4(1.0f, -1.0f, 1.0f, 0.0f));

	mesh.vertices.push_back(glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f));
	mesh.vertices.push_back(glm::vec4(1.0f, -1.0f, 1.0f, 0.0f));
	mesh.vertices.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));


	// screen quad VAO

	glGenVertexArrays(1, &QuadVAO);
	glGenBuffers(1, &QuadVBO);
	glBindVertexArray(QuadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices[0], GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * 4, &mesh.vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void Quad::Draw()
{
	glBindVertexArray(QuadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}