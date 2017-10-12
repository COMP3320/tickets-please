#include "menu.h"

Menu::Menu(GLFWwindow* window) {
	Menu::window = window;
}

void Menu::show() {
	visible = true;
}

void Menu::hide() {
	visible = false;
}

void Menu::toggleVisibility() {
	std::cout << "Toggled menu" << std::endl << std::endl;
	visible = !visible;
}

bool Menu::isVisible() {
	return visible;
}

void Menu::render() {
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}