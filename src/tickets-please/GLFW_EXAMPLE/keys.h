#ifndef KEYS_H
#define KEYS_H

#include <map>;
#include <vector>;
#include <GLFW/glfw3.h>
#include <iostream>

class Keys
{
public:
	Keys(GLFWwindow *window, std::vector<int> keys);
	bool wasPressed(int key);
	bool isPressed(int key);
	bool isJustPressed(int key);
	bool wasReleased(int key);
	bool isReleased(int key);
	bool isJustReleased(int key);
	void update();

private:
	GLFWwindow *window;
	std::map<int, bool> pressedKeys;
	std::vector<int> interestedKeys;
};

#endif