#include "Cylinder.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

#define PI (3.14f)

Cylinder::Cylinder()
	:RenderPrimitive()
{
	Height = 0;
	Radius = 0;
}

Cylinder::Cylinder(float H, float R, int Count)
	: RenderPrimitive()
{
	Height = H;
	Radius = R;
	SubDivCount = Count;
	bIsDoubleSided = true;
	bSmoothNormals = false;
}

Cylinder::~Cylinder()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(2, &VBO[0]);
	glDeleteBuffers(1, &VIB);
}

void Cylinder::Build()
{
	// Basic setup
	int VertexIndex = 0;
	int TriangleIndex = 0;

	// Make a cylinder section
	const float AngleBetweenQuads = (2.0f / (float)(SubDivCount)) * PI;
	const float DeltaX = 1.0f / (float)SubDivCount;
	glm::vec3 Offset = glm::vec3(0, Height, 0);

	// Start by building up vertices that make up the cylinder sides
	for (int QuadIndex = 0; QuadIndex < SubDivCount; QuadIndex++)
	{
		float Angle = (float)QuadIndex * AngleBetweenQuads;
		float NextAngle = (float)(QuadIndex + 1) * AngleBetweenQuads;

		float AngleDeg = Angle * 180.0f / PI;
		float NextAngleDeg = NextAngle * 180.0f / PI;

		// Set up the vertices
		glm::vec3 p0 = glm::vec3(glm::cos(Angle) * Radius, 0.f, glm::sin(Angle) * Radius);
		glm::vec3 p1 = glm::vec3(glm::cos(NextAngle) * Radius, 0.f, glm::sin(NextAngle) * Radius);
		glm::vec3 p2 = p1 + Offset;
		glm::vec3 p3 = p0 + Offset;

		// Set up the quad triangles
		int VertIndex1 = VertexIndex++;
		int VertIndex2 = VertexIndex++;
		int VertIndex3 = VertexIndex++;
		int VertIndex4 = VertexIndex++;

		mesh.vertices.push_back(p0);
		mesh.vertices.push_back(p1);
		mesh.vertices.push_back(p2);
		mesh.vertices.push_back(p3);


		// Now create two triangles from those four vertices
		mesh.indices.push_back(VertIndex4);
		mesh.indices.push_back(VertIndex3);
		mesh.indices.push_back(VertIndex1);

		mesh.indices.push_back(VertIndex3);
		mesh.indices.push_back(VertIndex2);
		mesh.indices.push_back(VertIndex1);


		// UVs.
		float u1, u2, u3, u4;
		u1 = (DeltaX * QuadIndex);
		u2 = (DeltaX * (QuadIndex + 1));
		u3 = (DeltaX * (QuadIndex + 1));
		u4 = (DeltaX * QuadIndex);

		float ViewStartAngle = 225; // Map the texture only from 225 deg to (225 + 80) deg
		float Fov = 80;
		float ViewEndAngle = ViewStartAngle + Fov;
		float DeltaAngle = 360.0f / SubDivCount;

		float UVMapStartSubDiv = ViewStartAngle / DeltaAngle;
		float UVMapEndSubDiv = ViewEndAngle / DeltaAngle;

		mesh.uv.push_back(glm::vec2((u1 * SubDivCount - UVMapStartSubDiv) / (UVMapEndSubDiv - UVMapStartSubDiv), 0.0f));
		mesh.uv.push_back(glm::vec2((u2 * SubDivCount - UVMapStartSubDiv) / (UVMapEndSubDiv - UVMapStartSubDiv), 0.0f));
		mesh.uv.push_back(glm::vec2((u3 * SubDivCount - UVMapStartSubDiv) / (UVMapEndSubDiv - UVMapStartSubDiv), 1.0f));
		mesh.uv.push_back(glm::vec2((u4 * SubDivCount - UVMapStartSubDiv) / (UVMapEndSubDiv - UVMapStartSubDiv), 1.0f));

		// Normals - 
		glm::vec3 NormalCurrent = glm::cross(mesh.vertices[VertIndex1] - mesh.vertices[VertIndex3], mesh.vertices[VertIndex2] - mesh.vertices[VertIndex3]);


		// If double sided, create extra polygons with different winding order
		if (bIsDoubleSided)
		{
			VertIndex1 = VertexIndex++;
			VertIndex2 = VertexIndex++;
			VertIndex3 = VertexIndex++;
			VertIndex4 = VertexIndex++;

			mesh.vertices.push_back(p0);
			mesh.vertices.push_back(p1);
			mesh.vertices.push_back(p2);
			mesh.vertices.push_back(p3);

			mesh.indices.push_back(VertIndex4);
			mesh.indices.push_back(VertIndex1);
			mesh.indices.push_back(VertIndex3);

			mesh.indices.push_back(VertIndex3);
			mesh.indices.push_back(VertIndex1);
			mesh.indices.push_back(VertIndex2);

			// UVs  
			float u1, u2, u3, u4;
			u1 = (DeltaX * QuadIndex);
			u2 = (DeltaX * (QuadIndex + 1));
			u3 = (DeltaX * (QuadIndex + 1));
			u4 = (DeltaX * QuadIndex);


			mesh.uv.push_back(glm::vec2((u1 * SubDivCount - UVMapStartSubDiv) / (UVMapEndSubDiv - UVMapStartSubDiv), 0.0f));
			mesh.uv.push_back(glm::vec2((u2 * SubDivCount - UVMapStartSubDiv) / (UVMapEndSubDiv - UVMapStartSubDiv), 0.0f));
			mesh.uv.push_back(glm::vec2((u3 * SubDivCount - UVMapStartSubDiv) / (UVMapEndSubDiv - UVMapStartSubDiv), 1.0f));
			mesh.uv.push_back(glm::vec2((u4 * SubDivCount - UVMapStartSubDiv) / (UVMapEndSubDiv - UVMapStartSubDiv), 1.0f));

		}

		/* Allocate and assign a Vertex Array Object to our handle */
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glGenBuffers(2, VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * 3 * sizeof(float), &mesh.vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);


		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, mesh.uv.size() * 2 * sizeof(float), &mesh.uv[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &VIB);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VIB);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

	}
}

void Cylinder::Draw()
{
	// render Cylinder
	glBindVertexArray(VAO);

	// bind textures on corresponding texture units
	if (GeomTexture != nullptr)
	{
		glActiveTexture(GeomTexture->GetTexUnit());
		glBindTexture(GL_TEXTURE_2D, GeomTexture->GetID());
	}

	// draw call
	glDrawElements( GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);


	glBindVertexArray(0);
}