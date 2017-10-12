#include "menu.h"

Menu::Menu(GLFWwindow* window) {
	Menu::window = window;

	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f,  1.0f, 0.0f,
	};

	// Generate 1 buffer, put the resulting identifier in vertexBuffer
	glGenBuffers(1, &vertexBuffer);

	// The following commands will talk about our 'vertexBuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	// Give our vertices to OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
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
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
	glDisableVertexAttribArray(0);
}