#ifndef MENU_H
#define MENU_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

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
	bool visible = false;
	GLFWwindow* window;
	GLuint vertexBuffer;
};

#endif