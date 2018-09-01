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

		//u1 = (u1 < 0.10f) ? 0.10f : u1;
		//u1 = (u1 > 0.40f) ? 0.40f : u1;

		//u2 = (u2 < 0.10f) ? 0.10f : u2;
		//u2 = (u2 > 0.40f) ? 0.40f : u2;

		//u3 = (u3 < 0.10f) ? 0.10f : u3;
		//u3 = (u3 > 0.40f) ? 0.40f : u3;

		//u4 = (u4 < 0.10f) ? 0.10f : u4;
		//u4 = (u4 > 0.40f) ? 0.40f : u4;

		float ViewStartAngle = 225;
		float Fov = 80;
		float ViewEndAngle = ViewStartAngle + Fov;
		float DeltaAngle = 360.0f / SubDivCount;

		float UVMapStartSubDiv = ViewStartAngle / DeltaAngle;
		float UVMapEndSubDiv = ViewEndAngle / DeltaAngle;

		mesh.uv.push_back(glm::vec2((u1 * SubDivCount - UVMapStartSubDiv) / (UVMapEndSubDiv - UVMapStartSubDiv), 0.0f));
		mesh.uv.push_back(glm::vec2((u2 * SubDivCount - UVMapStartSubDiv) / (UVMapEndSubDiv - UVMapStartSubDiv), 0.0f));
		mesh.uv.push_back(glm::vec2((u3 * SubDivCount - UVMapStartSubDiv) / (UVMapEndSubDiv - UVMapStartSubDiv), 1.0f));
		mesh.uv.push_back(glm::vec2((u4 * SubDivCount - UVMapStartSubDiv) / (UVMapEndSubDiv - UVMapStartSubDiv), 1.0f));

		// Normals
		glm::vec3 NormalCurrent = glm::cross(mesh.vertices[VertIndex1] - mesh.vertices[VertIndex3], mesh.vertices[VertIndex2] - mesh.vertices[VertIndex3]);

		if (bSmoothNormals)
		{
			// To smooth normals we give the vertices different values than the polygon they belong to.
			// GPUs know how to interpolate between those.
			// I do this here as an average between normals of two adjacent polygons
			float NextNextAngle = (float)(QuadIndex + 2) * AngleBetweenQuads;
			glm::vec3 p4 = glm::vec3(glm::cos(NextNextAngle) * Radius, glm::sin(NextNextAngle) * Radius, 0.f);

			// p1 to p4 to p2
			glm::vec3 NormalNext = glm::cross(p1 - p2, p4 - p2);// .GetSafeNormal();
			glm::vec3 AverageNormalRight = (NormalCurrent + NormalNext) * 0.5f;
			AverageNormalRight = AverageNormalRight;// .GetSafeNormal();

			float PreviousAngle = (float)(QuadIndex - 1) * AngleBetweenQuads;
			glm::vec3 pMinus1 = glm::vec3(glm::cos(PreviousAngle) * Radius, glm::sin(PreviousAngle) * Radius, 0.f);

			// p0 to p3 to pMinus1
			glm::vec3 NormalPrevious = glm::cross(p0 - pMinus1, p3 - pMinus1);// .GetSafeNormal();
			glm::vec3 AverageNormalLeft = (NormalCurrent + NormalPrevious) *0.5f;
			AverageNormalLeft = AverageNormalLeft;

			mesh.normals.push_back(AverageNormalLeft);
			mesh.normals.push_back(AverageNormalRight);
			mesh.normals.push_back(AverageNormalRight);
			mesh.normals.push_back(AverageNormalLeft);
		}
		else
		{
			// If not smoothing we just set the vertex normal to the same normal as the polygon they belong to
			mesh.normals.push_back(NormalCurrent);
			mesh.normals.push_back(NormalCurrent);
			mesh.normals.push_back(NormalCurrent);
			mesh.normals.push_back(NormalCurrent);
		}

		// Tangents (perpendicular to the surface)
		//glm::vec3 SurfaceTangent = p0 - p1;
		//SurfaceTangent = SurfaceTangent.GetSafeNormal();
		//InVertices[VertIndex1].Tangent = InVertices[VertIndex2].Tangent = InVertices[VertIndex3].Tangent = InVertices[VertIndex4].Tangent = FPackedNormal(SurfaceTangent);

		// -------------------------------------------------------
		// If double sided, create extra polygons but face the normals the other way.
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

			mesh.indices.push_back(VertIndex4);
			mesh.indices.push_back(VertIndex1);
			mesh.indices.push_back(VertIndex3);

			// UVs  
			float u1, u2, u3, u4;
			u1 = (DeltaX * QuadIndex);
			u2 = (DeltaX * (QuadIndex + 1));
			u3 = (DeltaX * (QuadIndex + 1));
			u4 = (DeltaX * QuadIndex);

			//u1 = (u1 < 0.10f) ? 0.10f : u1;
			//u1 = (u1 > 0.40f) ? 0.40f : u1;

			//u2 = (u2 < 0.10f) ? 0.10f : u2;
			//u2 = (u2 > 0.40f) ? 0.40f : u2;

			//u3 = (u3 < 0.10f) ? 0.10f : u3;
			//u3 = (u3 > 0.40f) ? 0.40f : u3;

			//u4 = (u4 < 0.10f) ? 0.10f : u4;
			//u4 = (u4 > 0.40f) ? 0.40f : u4;

			mesh.uv.push_back(glm::vec2((u1 * SubDivCount - UVMapStartSubDiv) / (UVMapEndSubDiv - UVMapStartSubDiv), 0.0f));
			mesh.uv.push_back(glm::vec2((u2 * SubDivCount - UVMapStartSubDiv) / (UVMapEndSubDiv - UVMapStartSubDiv), 0.0f));
			mesh.uv.push_back(glm::vec2((u3 * SubDivCount - UVMapStartSubDiv) / (UVMapEndSubDiv - UVMapStartSubDiv), 1.0f));
			mesh.uv.push_back(glm::vec2((u4 * SubDivCount - UVMapStartSubDiv) / (UVMapEndSubDiv - UVMapStartSubDiv), 1.0f));


			//mesh.uv.push_back(glm::vec2((u1 - 0.15f) * 5.0f, 0.0f));
			//mesh.uv.push_back(glm::vec2((u2 - 0.15f) * 5.0f, 0.0f));
			//mesh.uv.push_back(glm::vec2((u3 - 0.15f) * 5.0f, 1.0f));
			//mesh.uv.push_back(glm::vec2((u4 - 0.15f) * 5.0f, 1.0f));


			// Just simple (unsmoothed) normal for these
			//InVertices[VertIndex1].Normal = InVertices[VertIndex2].Normal = InVertices[VertIndex3].Normal = InVertices[VertIndex4].Normal = FPackedNormal(NormalCurrent);
			mesh.normals.push_back(NormalCurrent);
			mesh.normals.push_back(NormalCurrent);
			mesh.normals.push_back(NormalCurrent);
			mesh.normals.push_back(NormalCurrent);

			// Tangents (perpendicular to the surface)
			//glm::vec3 SurfaceTangentDbl = p0 - p1;
			//SurfaceTangentDbl = SurfaceTangentDbl.GetSafeNormal();
			//InVertices[VertIndex1].Tangent = InVertices[VertIndex2].Tangent = InVertices[VertIndex3].Tangent = InVertices[VertIndex4].Tangent = FPackedNormal(SurfaceTangentDbl);
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


		
		// -------------------------------------------------------
		// Caps are closed here by triangles that start at 0, then use the points along the circle for the other two corners.
		// A better looking method uses a vertex in the center of the circle, but uses two more polygons.  We will demonstrate that in a different sample.
		//if (QuadIndex != 0 && bInCapEnds)
		//{
		//	// Bottom cap
		//	glm::vec3 capVertex0 = glm::vec3(glm::cos(0) * Radius, glm::sin(0) * Radius, 0.f);
		//	glm::vec3 capVertex1 = glm::vec3(glm::cos(Angle) * Radius, glm::sin(Angle) * Radius, 0.f);
		//	glm::vec3 capVertex2 = glm::vec3(glm::cos(NextAngle) * Radius, glm::sin(NextAngle) * Radius, 0.f);

		//	VertIndex1 = VertexIndex++;
		//	VertIndex2 = VertexIndex++;
		//	VertIndex3 = VertexIndex++;
		//	InVertices[VertIndex1].Position = capVertex0;
		//	InVertices[VertIndex2].Position = capVertex1;
		//	InVertices[VertIndex3].Position = capVertex2;

		//	InTriangles[TriangleIndex++] = VertIndex1;
		//	InTriangles[TriangleIndex++] = VertIndex2;
		//	InTriangles[TriangleIndex++] = VertIndex3;

		//	InVertices[VertIndex1].UV0 = glm::vec2(0.5f - (glm::cos(0) / 2.0f), 0.5f - (glm::sin(0) / 2.0f));
		//	InVertices[VertIndex2].UV0 = glm::vec2(0.5f - (glm::cos(-Angle) / 2.0f), 0.5f - (glm::sin(-Angle) / 2.0f));
		//	InVertices[VertIndex3].UV0 = glm::vec2(0.5f - (glm::cos(-NextAngle) / 2.0f), 0.5f - (glm::sin(-NextAngle) / 2.0f));

		//	glm::vec3 CapNormalCurrent = glm::cross(InVertices[VertIndex1].Position - InVertices[VertIndex3].Position, InVertices[VertIndex2].Position - InVertices[VertIndex3].Position).GetSafeNormal();
		//	InVertices[VertIndex1].Normal = InVertices[VertIndex2].Normal = InVertices[VertIndex3].Normal = FPackedNormal(CapNormalCurrent);

		//	// Tangents (perpendicular to the surface)
		//	glm::vec3 SurfaceTangentCap = p0 - p1;
		//	SurfaceTangentCap = SurfaceTangentCap.GetSafeNormal();
		//	InVertices[VertIndex1].Tangent = InVertices[VertIndex2].Tangent = InVertices[VertIndex3].Tangent = FPackedNormal(SurfaceTangentCap);

		//	// Top cap
		//	capVertex0 = capVertex0 + Offset;
		//	capVertex1 = capVertex1 + Offset;
		//	capVertex2 = capVertex2 + Offset;

		//	VertIndex1 = VertexIndex++;
		//	VertIndex2 = VertexIndex++;
		//	VertIndex3 = VertexIndex++;
		//	InVertices[VertIndex1].Position = capVertex0;
		//	InVertices[VertIndex2].Position = capVertex1;
		//	InVertices[VertIndex3].Position = capVertex2;

		//	InTriangles[TriangleIndex++] = VertIndex3;
		//	InTriangles[TriangleIndex++] = VertIndex2;
		//	InTriangles[TriangleIndex++] = VertIndex1;

		//	InVertices[VertIndex1].UV0 = glm::vec2(0.5f - (glm::cos(0) / 2.0f), 0.5f - (glm::sin(0) / 2.0f));
		//	InVertices[VertIndex2].UV0 = glm::vec2(0.5f - (glm::cos(Angle) / 2.0f), 0.5f - (glm::sin(Angle) / 2.0f));
		//	InVertices[VertIndex3].UV0 = glm::vec2(0.5f - (glm::cos(NextAngle) / 2.0f), 0.5f - (glm::sin(NextAngle) / 2.0f));

		//	CapNormalCurrent = glm::cross(InVertices[VertIndex1].Position - InVertices[VertIndex3].Position, InVertices[VertIndex2].Position - InVertices[VertIndex3].Position).GetSafeNormal();
		//	InVertices[VertIndex1].Normal = InVertices[VertIndex2].Normal = InVertices[VertIndex3].Normal = FPackedNormal(CapNormalCurrent);

		//	// Tangents (perpendicular to the surface)
		//	SurfaceTangentCap = p0 - p1;
		//	SurfaceTangentCap = SurfaceTangentCap.GetSafeNormal();
		//	InVertices[VertIndex1].Tangent = InVertices[VertIndex2].Tangent = InVertices[VertIndex3].Tangent = FPackedNormal(SurfaceTangentCap);
		//}
	}
}

void Cylinder::Draw()
{
	// render Cylinder
	glBindVertexArray(VAO);
	glDrawElements( GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, (void*)0);


	glBindVertexArray(0);
}