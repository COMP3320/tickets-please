#include "game.h"

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Game
Game* game = NULL;

// Callbacks
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xPosition, double yPosition);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

// ------------------------------------

int main()
{
	game = new Game(SCR_WIDTH, SCR_HEIGHT);

	game->setup();
	
	glfwSetFramebufferSizeCallback(game->getWindow(), framebufferSizeCallback);
	glfwSetCursorPosCallback(game->getWindow(), mouseCallback);
	glfwSetScrollCallback(game->getWindow(), scrollCallback);

	return game->run();
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	game->onFrameBufferResize(width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouseCallback(GLFWwindow* window, double xPosition, double yPosition)
{
	game->onMouseMoved(xPosition, yPosition);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	game->onMouseScrolled(xOffset, yOffset);
}