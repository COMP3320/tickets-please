#ifndef GAME_H
#define GAME_H

#define GLM_FORCE_RADIANS

#include <ctime> 
#include <fstream>
#include <iostream>
#include <stdio.h>  
#include <stdlib.h> 
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>

#include "camera.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "menu.h"
#include "model.h"
#include "shader.h"
//#include "sound.h"

class Game
{
public:
	// Constructors
	Game(int width = 800, int height = 600);

	// Actions
	bool setup();
	int run();

	// Getters
	GLFWwindow* getWindow();

	// Callbacks
	void onFrameBufferResize(int width, int height);
	void onMouseMoved(double xpos, double ypos);
	void onMouseScrolled(double xoffset, double yoffset);
private:
	// Window
	GLFWwindow* window;
	int width;
	int height;

	// Timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	// Camera
	Camera camera;
	float lastX;
	float lastY;
	bool firstMouse;

	// Menu
	Menu* menu = NULL;
	bool wasEscapePressed;

	// Skybox
	unsigned int skyboxVAO;
	unsigned int skyboxVBO;
	unsigned int cubemapTexture;

	// Shaders
	Shader* shader;
	Shader* skyboxShader;

	// Models
	Model* ourModel3;
	Model* ourModel4;
	Model* ourModel5;
	Model* ourModel6;

	// Methods
	bool setupGLFW();
	bool setupGLEW();
	void setupCamera();
	void setupMenu();
	void setupCubemap();
	void setupState();
	void setupSkybox();
	void setupShaders();
	void loop();
	void updateTime();
	void processInput();
	void render();
	void prepareOutput();
	void sendOutput();
	void cleanup();
	unsigned int loadCubemap(std::vector<std::string> faces);
};

#endif