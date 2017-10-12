#ifndef MENU_H
#define MENU_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>

#include <ctime> 
#include <stdio.h>  
#include <stdlib.h> 
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <iostream>
#include <string>
#include <vector>

class Menu
{
public:
	Menu(GLFWwindow* window);
	void show();
	void hide();
	void toggleVisibility();
	bool isVisible();
	void render();

private:
	bool visible;
	GLFWwindow* window;
	GLuint vertexBuffer;
	GLuint programId;
};

#endif