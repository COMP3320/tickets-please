#pragma once

#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>

#include <GL/glew.h>  

//Include GLFW  
#include <GLFW/glfw3.h>  

//Include matrix libraries
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "stb_image_aug.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//Include helper libraries
#include "Shader.h"
#include "Mesh.h"

class Model{
public:
	/* Pub Mem Vars */
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammaCorrection;

	/* Pub Funcs */
	Model(std::string path, bool gamma = false);
	void Draw(Shader shader);

private:
	void loadModel(std::string const &path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif

