#include "mesh.h"

Mesh::Mesh()
{
	vertices.clear();
	uv.clear();
	normals.clear();
}

Mesh::~Mesh()
{
	vertices.clear();
	uv.clear();
	normals.clear();
}

void Mesh::SetVertices(std::vector<glm::vec3>& Vertices)
{
	vertices = Vertices;
}
void Mesh::SetUVs(std::vector<glm::vec2>& UVs)
{
	uv = UVs;
}
void Mesh::SetNormals(std::vector<glm::vec3>& Normals)
{
	normals = Normals;
}
