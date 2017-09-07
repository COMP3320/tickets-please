#pragma once

#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include <sstream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <assimp/types.h>

#include "Shader.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct Texture {
	unsigned int id;
	std::string type;
	aiString path;
};

class Mesh{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	Mesh(std::vector<Vertex>		verticesIn, 
		std::vector<unsigned int>	indicesIn, 
		std::vector<Texture>		texturesIn);
	void Draw(Shader shader);
private:
	unsigned int vao, vbo, ebo;
	void setupMesh();
};

#endif
