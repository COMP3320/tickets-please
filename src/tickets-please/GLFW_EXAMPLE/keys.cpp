#include "keys.h"

Keys::Keys(GLFWwindow *window, std::vector<int> keys)
{
	this->window = window;
	this->interestedKeys = keys;

	for (std::vector<int>::iterator iterator = interestedKeys.begin(); iterator != interestedKeys.end(); ++iterator) {
		int key = *iterator;
		bool pressed = glfwGetKey(window, key) == GLFW_PRESS;

		pressedKeys.insert(std::pair<int, bool>(key, pressed));
	}
}

bool Keys::wasPressed(int key)
{
	return pressedKeys.find(key)->second;
}

bool Keys::isPressed(int key)
{
	return glfwGetKey(window, key) == GLFW_PRESS;
}

bool Keys::isJustPressed(int key)
{
	return this->wasReleased(key) && !this->wasPressed(key) && this->isPressed(key);
}

bool Keys::wasReleased(int key)
{
	return !pressedKeys.find(key)->second;
}

bool Keys::isReleased(int key)
{
	return glfwGetKey(window, key) == GLFW_RELEASE;
}

bool Keys::isJustReleased(int key)
{
	return this->wasPressed(key) && !this->wasReleased(key) && this->isReleased(key);
}

void Keys::update()
{
	for (std::vector<int>::iterator iterator = interestedKeys.begin(); iterator != interestedKeys.end(); ++iterator) {
		int key = *iterator;
		bool pressed = (glfwGetKey(window, key) == GLFW_PRESS);

		pressedKeys[key] = pressed;
	}
}