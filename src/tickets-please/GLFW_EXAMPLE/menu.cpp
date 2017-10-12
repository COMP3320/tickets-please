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
	std::cout << "Toggled" << std::endl << std::endl;
	visible = !visible;
}

bool Menu::isVisible() {
	return visible;
}

void Menu::render() {

}