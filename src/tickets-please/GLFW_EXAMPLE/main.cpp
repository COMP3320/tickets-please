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
#include "pickRay.h"

#include <stdio.h>  
#include <stdlib.h> 

#include <iostream>
#include <fstream> //fstream
#include <ctime> 

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, BoundBox areaMap, BoundBox bb[], int arrLength);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void renderSelection(void);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, -4.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float currX, currY;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// store models in a model map!
struct modelContainer {
	Model model;
	glm::mat4 transform;
	bool rendered = true;
	InteractType type = NONE;
	int code;
};
std::map<std::string, modelContainer> modelMap;

// store model transforms in a transform map!
//std::map<std::string, glm::mat4> transMap;

// store lights in a struct
struct Light {
	glm::vec4 colour;
	glm::vec4 position;
};

Shader selection;

/*
 *	TEST GLOBALS FOR PERSON INTERACTION
 */

int code = 0;
bool flag = false;

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

void constructScene(Shader s) {
	//s.use();
	for (auto it = modelMap.begin(); it != modelMap.end(); it++) {
		if ((it->second).rendered) {
			if (s.ID == selection.ID) { s.setInt("code", (it->second).code); }
			s.setMat4("model", modelMap[it->first].transform);
			(it->second).model.Draw(s);
		}
	}
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
	glfwSetMouseButtonCallback(window, mouse_button_callback);

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
	//glEnable(GL_REPLACE);

//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// build and compile shaders
	// -------------------------
	
	Shader ourShader("shader.vert", "shader.frag");
	Shader skyboxShader("cubemap.vert", "cubemap.frag");
	selection.init("selection.vert", "selection.frag");

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
	//modelMap["train"]   = Model("../objects/MapDemo2.obj");
	/*
	modelMap["chairs1"] = modelContainer();
	modelMap["chairs2"] = modelContainer();
	modelMap["chairs3"] = modelContainer();
	modelMap["chairs4"] = modelContainer();
	modelMap["person1"] = modelContainer();
	modelMap["ticket"]  = modelContainer();
	modelMap["id"]      = modelContainer();
	*/
	modelMap["chairs1"].model = Model("../objects/chairTest.obj");
	modelMap["chairs2"].model = Model("../objects/chairTest.obj");
	modelMap["chairs3"].model = Model("../objects/chairTest.obj");
	modelMap["chairs4"].model = Model("../objects/chairTest.obj");
	modelMap["person1"].model = Model("../objects/person.obj");
	modelMap["ticket"].model  = Model("../objects/Ticket.obj");
	modelMap["id"].model	  = Model("../objects/ID.obj");

	modelMap["ticket"].rendered = false;
	modelMap["id"].rendered = false;

	//Model ticket("../objects/Ticket.obj");
	//Model id("../objects/ID.obj");

	// set models in scene
	//glm::mat4 train_mat;
	//train_mat = glm::translate(train_mat, glm::vec3(0.0f, -1.0f, -4.5f));
	//modelMap["train"].transform = train_mat;

	glm::mat4 chairs1_mat;
	chairs1_mat = glm::rotate(chairs1_mat, 1.6f, glm::vec3(0.0f, 1.0f, 0.0f));
	chairs1_mat = glm::translate(chairs1_mat, glm::vec3(7.0f, -3.0f, 4.5f));
	modelMap["chairs1"].transform = chairs1_mat;

	glm::mat4 chairs2_mat;
	chairs2_mat = glm::rotate(chairs2_mat, 4.725f, glm::vec3(0.0f, 1.0f, 0.0f));
	chairs2_mat = glm::translate(chairs2_mat, glm::vec3(-2.0f, -3.0f, 4.0f));
	modelMap["chairs2"].transform = chairs2_mat;

	glm::mat4 chairs3_mat;
	chairs3_mat = glm::rotate(chairs3_mat, 4.725f, glm::vec3(0.0f, 1.0f, 0.0f));
	chairs3_mat = glm::translate(chairs3_mat, glm::vec3(-2.0f, -3.0f, -4.25f));
	modelMap["chairs3"].transform = chairs3_mat;

	glm::mat4 chairs4_mat;
	chairs4_mat = glm::rotate(chairs4_mat, 1.6f, glm::vec3(0.0f, 1.0f, 0.0f));
	chairs4_mat = glm::translate(chairs4_mat, glm::vec3(7.0f, -3.0f, -4.0f));
	modelMap["chairs4"].transform = chairs4_mat;

	glm::mat4 person1_mat;
	person1_mat = glm::scale(person1_mat, glm::vec3(0.4f, 0.4f, 0.4f));
	person1_mat = glm::translate(person1_mat, glm::vec3(14.0f, -7.5f, -11.5f));
	person1_mat = glm::rotate(person1_mat, 1.55f, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMap["person1"].transform = person1_mat;

	glm::mat4 ticket_mat;
	ticket_mat = glm::translate(ticket_mat, glm::vec3(4.5f, 0.0f, -5.0f));
	ticket_mat = glm::scale(ticket_mat, glm::vec3(0.1f, 0.1f, 0.1f));
	modelMap["ticket"].transform = ticket_mat;

	glm::mat4 id_mat;
	id_mat = glm::translate(id_mat, glm::vec3(4.5f, 0.0f, -4.25f));
	id_mat = glm::scale(id_mat, glm::vec3(0.1f, 0.1f, 0.1f));
	modelMap["id"].transform = id_mat;

	// set models interactive type
	int i = 100;
	for (auto it = modelMap.begin(); it != modelMap.end(); it++, i++) {
		(it->second).code = i;
		std::string typeStr = (it->first).substr(0, 6);
		if (typeStr == "chairs")		{ (it->second).type = CHAIR;  }
		else if (typeStr == "person")	{ (it->second).type = PERSON; }
		else							{ (it->second).type = NONE; }
	}

	// load bounding boxes

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

	unsigned char res[4] = {0,0,0,0};
	GLint viewport[4];
	glm::vec4 origColour = glm::vec4(0.9f, 0.95f, 1.0f, 0.95f);
	Light lights[4];
	int RANGE = 15;
	for (int i = 0; i < 2; ++i) {
		lights[i].colour   = origColour;
	}
	lights[0].position = glm::vec4(rand() * -RANGE, 0.0f, 3.0f, 1.0f);
	lights[1].position = glm::vec4(rand() * -RANGE, 0.0f, 3.0f, 1.0f);
	lights[2].position = glm::vec4(1.0f, 0.0f, 3.0f, 1.0f);
	lights[3].position = glm::vec4(-1.0f, 0.0f, 3.0f, 1.0f);
	int numFrames = 0;
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		//std::cout << camera.Position[0] << ", " << camera.Position[1] << ", " << camera.Position[2] << std::endl;
		numFrames++;
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		//if (numFrames % 20 == 0) {
			for (int i = 0; i < 2; ++i) {
				if (lights[i].position.x >= -RANGE) {
					lights[i].position.x -= 0.09f;
				}
				else {
					lights[i].position.x = RANGE;
				}
				lights[i].colour = origColour;
			}
		//}
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window, areaMap, bb, sizeof(bb) / sizeof(bb[0]));

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ourShader.use();
		//glm::vec4 light_pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glUniform4fv(glGetUniformLocation(ourShader.ID, "light_position[0]"),	1, glm::value_ptr(lights[0].position));
		glUniform4fv(glGetUniformLocation(ourShader.ID, "light_position[1]"),	1, glm::value_ptr(lights[1].position));
		glUniform4fv(glGetUniformLocation(ourShader.ID, "light_position[2]"),	1, glm::value_ptr(lights[2].position));
		glUniform4fv(glGetUniformLocation(ourShader.ID, "light_position[3]"),	1, glm::value_ptr(lights[3].position));
		glUniform4fv(glGetUniformLocation(ourShader.ID, "light_colour[0]"),		1, glm::value_ptr(lights[0].colour));
		glUniform4fv(glGetUniformLocation(ourShader.ID, "light_colour[1]"),		1, glm::value_ptr(lights[1].colour));
		glUniform4fv(glGetUniformLocation(ourShader.ID, "light_colour[2]"),		1, glm::value_ptr(lights[2].colour));
		glUniform4fv(glGetUniformLocation(ourShader.ID, "light_colour[3]"),		1, glm::value_ptr(lights[3].colour));
		/*
		ourShader.setVec4("light_position", lights[0].position);
		ourShader.setVec4("light_position", lights[1].position);
		ourShader.setVec4("light_colour", lights[0].colour);
		ourShader.setVec4("light_colour", lights[1].colour);
		*/
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		constructScene(ourShader);

		if (flag == true)
		{
			switch (code)
			{
				case 104:
					/*
					ourShader.setMat4("model", ticket_mat);
					ticket.Draw(ourShader);

					
					ourShader.setMat4("model", id_mat);
					id.Draw(ourShader);
					*/
					camera.Position.x = 3.079;
					camera.Position.y = 0;
					camera.Position.z = -4.62014;

					camera.Yaw = 0.0f;
					camera.Pitch = 0.0f;
					camera.updateCameraVectors();

					//		flag = false;
				break;
			}
		}

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
/**/	
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		//glGetIntegerv(GL_VIEWPORT, viewport);
		//std::cout << "CurrX: " << currX << " CurrY: " << currY << " PixelY: " << viewport[3] - currY << std::endl;
		//glReadPixels(SCR_WIDTH/2, SCR_HEIGHT/2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &res);
		/*
		if (int(res[0]) != 0) {
			std::cout << (int)res[0] << std::endl;
		}
		*/
		/*
		glEnable(GL_DEPTH);
		glEnable(GL_BLEND);

		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		ourShader.use();
		constructScene(ourShader);
		*/

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
		camera.ProcessKeyboard(FORWARD, deltaTime * speed, areaMap, bb, arrLength, flag);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime * speed, areaMap, bb, arrLength, flag);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime * speed, areaMap, bb, arrLength, flag);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime * speed, areaMap, bb, arrLength, flag);
}

void mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{
	unsigned char res[4];
	GLint viewport[4];
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (flag == false)
		{
			renderSelection();
			glGetIntegerv(GL_VIEWPORT, viewport);
			glReadPixels(SCR_WIDTH / 2, SCR_HEIGHT / 2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &res);

			std::cout << (int)res[0] << std::endl;
			std::string modelStr;
			//Model focusModel;
			modelContainer m;
			for (auto it = modelMap.begin(); it != modelMap.end(); it++) {
				if ((it->second).code == *res) {
					modelStr = it->first;
				}
			}
			if (glm::distance(camera.Position, glm::vec3(m.transform[3])) < 7.0f) {
				switch (modelMap[modelStr].type) {
				case NONE:
					std::cout << modelMap[modelStr].code << ": Pretty, but uninteractable." << std::endl;
					break;
				case PERSON:
					std::cout << modelMap[modelStr].code << ": How's it going?" << std::endl;
					flag = true;
					modelMap["ticket"].rendered = true;
					modelMap["id"].rendered = true;
					code = modelMap[modelStr].code;
					break;
				case CHAIR:
					std::cout << modelMap[modelStr].code << ": I'll sit down." << std::endl;
					camera.Position = glm::vec3((modelMap[modelStr].transform)[3]);
					camera.Position[1] = -0.5;
					camera.setSitting();
					break;
				}
			}
		}
		else
		{
			flag = false;
			modelMap["ticket"].rendered = false;
			modelMap["id"].rendered = false;
		}
		
	}
		
}

void renderSelection(void)
{
	unsigned char res[4];
	GLint viewport[4];
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	selection.use();

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	selection.setMat4("projection", projection);
	selection.setMat4("view", view);
	
	constructScene(selection);

	glClearColor(0.00f, 0.00f, 1.0f, 0.0f);
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
	currX = xpos;
	currY = ypos;
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

	camera.ProcessMouseMovement(xoffset, yoffset, flag);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

