#include "shader.h"

Shader::Shader(std::string vertPath, std::string fragPath){
	// Load in file contents
	std::ifstream in(vertPath);
	std::string contents((std::istreambuf_iterator<char>(in)),
						  std::istreambuf_iterator<char>());
	const char* vertSource = contents.c_str();

	// Compile vert shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertSource, NULL);
	glCompileShader(vertexShader);

	// Check vert shader compiled
	getShaderCompileStatus(vertexShader);

	std::ifstream in2(fragPath);
	std::string contents2((std::istreambuf_iterator<char>(in2)),
						   std::istreambuf_iterator<char>());
	const char* fragSource = contents2.c_str();
	
	// Compile frag shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragSource, NULL);
	glCompileShader(fragmentShader);

	// Check that frag shader compiled
	getShaderCompileStatus(fragmentShader);

	// Create shader program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
}

bool Shader::getShaderCompileStatus(GLuint shader) {
	//Get status
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE) {
		return true;
	}
	else {
		//Get log
		char buffer[512];
		glGetShaderInfoLog(shader, 512, NULL, buffer);
		std::cout << buffer << std::endl;
		return false;
	}
}



GLint Shader::getAttribLocation(GLchar* id) {
	return glGetAttribLocation(shaderProgram, id);
}

GLint Shader::getUniformLocation(GLchar* id) {
	return glGetUniformLocation(shaderProgram, id);
}

const GLuint Shader::getID() {
	return shaderProgram;
}

void Shader::bind() {
	glUseProgram(shaderProgram);
}

void Shader::unbind() {
	glUseProgram(0);
}