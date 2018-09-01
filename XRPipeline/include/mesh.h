#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Mesh
{
public:
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> normals;

	Mesh();
	~Mesh();
	void SetVertices(std::vector<glm::vec3>& Vertices);
	void SetUVs(std::vector<glm::vec2>& UVs);
	void SetNormals(std::vector<glm::vec3>& Normals);

};
