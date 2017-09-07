#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <iostream> //cout
#include <fstream> //fstream
#include <stdio.h>  
#include <stdlib.h> 

#include <GL/glew.h> 

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader {
public:
	Shader(std::string fragPath, std::string vertPath);
	bool getShaderCompileStatus(GLuint shader);
	void bind();
	void unbind();
	GLint getAttribLocation(GLchar* id);
	GLint getUniformLocation(GLchar* id);
	const GLuint getID();
private:
	GLuint shaderProgram;
};

#endif