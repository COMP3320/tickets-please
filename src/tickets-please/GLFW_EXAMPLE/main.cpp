#define GLM_FORCE_RADIANS
#include <SOIL.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "cubemapVert.h"
#include "boundbox.h"

#include <stdio.h>  
#include <stdlib.h> 

#include <iostream>
#include <fstream> //fstream
#include <ctime> 

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, BoundBox areaMap, BoundBox bb[], int arrLength);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, -4.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int loadCubemap(std::vector<std::string> faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char *data = SOIL_load_image(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			SOIL_free_image_data(data);
		}
		else {
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			SOIL_free_image_data(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tickets, Please", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glewExperimental = GL_TRUE;
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	GLenum err = glewInit();

	//If GLEW hasn't initialized  
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return -1;
	}

	// get cubemap texture
	std::vector<std::string> faces = {
		"../objects/skybox/right.tga",
		"../objects/skybox/left.tga",
		"../objects/skybox/top.tga",
		"../objects/skybox/bottom.tga",
		"../objects/skybox/back.tga",
		"../objects/skybox/front.tga"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_REPLACE);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// build and compile shaders
	// -------------------------
	Shader ourShader("shader.vert", "shader.frag");
	Shader skyboxShader("cubemap.vert", "cubemap.frag");

	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// load models
	// -----------

	Model ourModel("../objects/MapDemo2.obj");
	Model chairSet1("../objects/chairTest.obj");
	Model chairSet2("../objects/chairTest.obj");
	Model chairSet3("../objects/chairTest.obj");
	Model chairSet4("../objects/chairTest.obj");
	Model person("../objects/person.obj");
	BoundBox bb[5] = {	BoundBox(glm::vec3(7.1f, 2.0f, -5.7f), glm::vec3(1.3, -2.0f, -7.8f)),
						BoundBox(glm::vec3(-1.1f, 2.0f, -5.7f), glm::vec3(-7.3, -2.0f, -7.8f)),
						BoundBox(glm::vec3(-1.1f, 2.0f, -1.2f), glm::vec3(-7.3, -2.0f, -3.0f)),
						BoundBox(glm::vec3(7.1f, 2.0f, -1.2f), glm::vec3(1.3f, -2.0f, -3.0f)),
						BoundBox(glm::vec3(6.6f, 2.0f, -3.3f), glm::vec3(5.0f, -2.0f, -5.7f))
	};

	BoundBox areaMap(glm::vec3(7.75f, 2.0f, -1.2f), glm::vec3(-7.75f, -2.0f, -7.8f));
//	BoundBox areaMap(glm::vec3(20.75f, 2.0f, 20.2f), glm::vec3(-20.75f, -2.0f, -20.8f));
//	BoundBox bb(ourModel.getMaxCords(), ourModel.getMinCords());
	// don't forget to enable shader before setting uniforms
	ourShader.use();
	skyboxShader.use();
	ourShader.setInt("texture1", 0);
	skyboxShader.setInt("skybox", 0);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window, areaMap, bb, sizeof(bb)/sizeof(bb[0]));

		// render
		// ------
		glClearColor(0.00f, 0.00f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.use();
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, -4.5f));
	//	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		ourShader.setMat4("model", model);
		ourModel.Draw(ourShader);

		glm::mat4 model2;
		model2 = glm::rotate(model2, 1.6f, glm::vec3(0.0f, 1.0f, 0.0f));
		model2 = glm::translate(model2, glm::vec3(7.0f, -3.0f, 4.5f));
		ourShader.setMat4("model", model2);
		chairSet1.Draw(ourShader);

		glm::mat4 model3;
		model3 = glm::rotate(model3, 4.725f, glm::vec3(0.0f, 1.0f, 0.0f));
		model3 = glm::translate(model3, glm::vec3(-2.0f, -3.0f, 4.0f));
		ourShader.setMat4("model", model3);
		chairSet2.Draw(ourShader);

		glm::mat4 model4;
		model4 = glm::rotate(model4, 4.725f, glm::vec3(0.0f, 1.0f, 0.0f));
		model4 = glm::translate(model4, glm::vec3(-2.0f, -3.0f, -4.25f));
		ourShader.setMat4("model", model4);
		chairSet3.Draw(ourShader);

		glm::mat4 model5;
		model5 = glm::rotate(model5, 1.6f, glm::vec3(0.0f, 1.0f, 0.0f));
		model5 = glm::translate(model5, glm::vec3(7.0f, -3.0f, -4.0f));
		ourShader.setMat4("model", model5);
		chairSet4.Draw(ourShader);

		glm::mat4 model6;
		model6 = glm::scale(model6, glm::vec3(0.4f, 0.4f, 0.4f));
		model6 = glm::translate(model6, glm::vec3(14.0f, -7.5f, -11.5f));
		model6 = glm::rotate(model6, 1.55f, glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.setMat4("model", model6);
		person.Draw(ourShader);

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVAO);

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, BoundBox areaMap, BoundBox bb[], int arrLength)
{
	int speed = 1;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		speed = 2;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.setCrouch(true);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
		camera.setCrouch(false);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime * speed, areaMap, bb, arrLength);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime * speed, areaMap, bb, arrLength);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime * speed, areaMap, bb, arrLength);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime * speed, areaMap, bb, arrLength);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

