#define GLM_FORCE_RADIANS
#include <SOIL.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "cubemapVert.h"
#include "boundbox.h"
#include "pickRay.h"
#include "text2d.h"
#include "motionBlur.h"
#include "godRays.h"
#include "grayscale.h"
#include "inversion.h"
#include "keys.h"

#include <stdio.h>  
#include <stdlib.h> 

#include <iostream>
#include <fstream> //fstream
#include <ctime> 

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, BoundBox areaMap, BoundBox bb[], int arrLength, Keys keys);
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

float reposx, reposy, yupdate = 0;
float canx, cany, canz;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int score = 0;

// store models in a model map!
struct modelContainer {
	Model model;
	glm::mat4 transform;
	bool rendered = true;
	InteractType type = NONE;
	int code;
	std::string linkedID, linkedTicket;
	std::string idName, ticketName;
};
std::map<std::string, modelContainer> modelMap;
std::string focusPerson;

// store model transforms in a transform map!
//std::map<std::string, glm::mat4> transMap;

// store lights in a struct
struct Light {
	glm::vec4 colour;
	glm::vec4 position;
};

Shader selection;

bool enableDepthOfField = false;

MotionBlur* motionBlur;
GodRays* godRays;
Grayscale* grayscale;
Inversion* inversion;

/*
 *	TEST GLOBALS FOR PERSON INTERACTION
 */

int code = 0;
bool flag = false;

bool moveFlag = false;
std::string moveModel = "";

std::string modelSelect(int pickCode)
{
	switch (pickCode)
	{
	case 102:
		return "chairs3";
		break;
	}
}

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

void generateTickets() {

}

int main()
{
	srand(time(0));
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
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

	// create transforms
	// -----------
	//Create chair transforms
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
	//Create person transforms
	glm::mat4 person1_mat;
	person1_mat = glm::scale(person1_mat, glm::vec3(0.4f, 0.4f, 0.4f));
	person1_mat = glm::rotate(person1_mat, 1.55f, glm::vec3(0.0f, 1.0f, 0.0f));
	person1_mat = glm::translate(person1_mat, glm::vec3(12.0f, -7.5f, 13.5f));
	modelMap["person1"].transform = person1_mat;

	glm::mat4 person2_mat;
	person2_mat = glm::scale(person2_mat, glm::vec3(0.4f, 0.4f, 0.4f));
	person2_mat = glm::rotate(person2_mat, -1.55f, glm::vec3(0.0f, 1.0f, 0.0f));
	person2_mat = glm::translate(person2_mat, glm::vec3(-12.0f, -7.5f, 13.5f));
	modelMap["person2"].transform = person2_mat;
	//Create ticket and ID transforms
	glm::mat4 ticket1_mat;
	ticket1_mat = glm::scale(ticket1_mat, glm::vec3(0.1f, 0.1f, 0.1f));
	ticket1_mat = glm::translate(ticket1_mat, glm::vec3(45.0f, 0.0f, -50.0f));
	modelMap["ticket1"].transform = ticket1_mat;

	glm::mat4 id1_mat;
	id1_mat = glm::scale(id1_mat, glm::vec3(0.1f, 0.1f, 0.1f));
	id1_mat = glm::translate(id1_mat, glm::vec3(45.0f, 0.0f, -42.5f));
	modelMap["id1"].transform = id1_mat;

	glm::mat4 ticket2_mat;
	ticket2_mat = glm::scale(ticket2_mat, glm::vec3(0.1f, 0.1f, 0.1f));
	ticket2_mat = glm::rotate(ticket2_mat, -3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
	ticket2_mat = glm::translate(ticket2_mat, glm::vec3(45.0f, 0.0f, 50.0f));
	modelMap["ticket2"].transform = ticket2_mat;

	glm::mat4 id2_mat;
	id2_mat = glm::scale(id2_mat, glm::vec3(0.1f, 0.1f, 0.1f));
	id2_mat = glm::rotate(id2_mat, -3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
	id2_mat = glm::translate(id2_mat, glm::vec3(45.0f, 0.0f, 42.5f));
	modelMap["id2"].transform = id2_mat;

	glm::mat4 can_mat;
	can_mat = glm::translate(can_mat, glm::vec3(0.0f, -2.55f, -7.0f));
	can_mat = glm::scale(can_mat, glm::vec3(0.1f, 0.1f, 0.1f));
	modelMap["can"].transform = can_mat;

	canx = 0.0f;
	cany = -2.55f;
	canz = -7.0f;

	glm::mat4 map_mat;
	map_mat = glm::translate(map_mat, glm::vec3(0.0f, -1.0f, -4.5f));
	modelMap["train"].transform = map_mat;

	glm::mat4 person3_mat;
	person3_mat = glm::scale(person3_mat, glm::vec3(0.5f, 0.5f, 0.5f));
//	person3_mat = glm::rotate(person3_mat, -1.55f, glm::vec3(0.0f, 1.0f, 0.0f));
	person3_mat = glm::translate(person3_mat, glm::vec3(4.5f, -6.0f, -5.0f));
	modelMap["person3"].transform = person3_mat;

	// load models
	// -----------
	modelMap["chairs1"].model = Model("../objects/chairTest.obj", chairs1_mat);
		modelMap["chairs2"].model = Model("../objects/chairTest.obj", chairs2_mat);
	modelMap["chairs3"].model = Model("../objects/chairTest.obj", chairs3_mat);
		modelMap["chairs4"].model = Model("../objects/chairTest.obj", chairs4_mat);
	modelMap["person1"].model = Model("../objects/person/person.obj", person1_mat);
	modelMap["person2"].model = Model("../objects/person/person.obj", person2_mat);
	modelMap["person3"].model = Model("../objects/person/personsitting.obj", person3_mat);
	// Generate ticket and ids
	std::string docNames[] = {
		"junior_invalid",
		"junior_valid",
		"normal_invalid",
		"normal_valid",
		"senior_invalid",
		"senior_valid"
	};
	
	int ticketNum = rand() % 6;
	int idNum = 0;

	int reroll = rand();

	modelMap["ticket1"].model = Model(std::string("../objects/ticket/" + docNames[ticketNum] + ".obj"), ticket1_mat);
	modelMap["ticket1"].ticketName = docNames[ticketNum];
	if (reroll < 0.4) { idNum = int(rand() % 7); }
	else { idNum = ticketNum; }
	modelMap["id1"].model = Model(std::string("../objects/id/" + docNames[idNum] + ".obj"), id1_mat);
	modelMap["id1"].idName = docNames[idNum];

	ticketNum = rand() % 6;
	reroll = rand();
	modelMap["ticket2"].model = Model(std::string("../objects/ticket/" + docNames[ticketNum] + ".obj"), ticket2_mat);
	modelMap["ticket2"].ticketName = docNames[ticketNum];
	if (reroll < 0.4) { idNum = int(rand() % 7); }
	else { idNum = ticketNum; }
	modelMap["id2"].model = Model("../objects/id/" + docNames[idNum] + ".obj", id2_mat);
	modelMap["id2"].idName = docNames[idNum];

	modelMap["can"].model = Model("../objects/can.obj", can_mat);

	modelMap["train"].model = Model("../objects/MapDemo3.obj", map_mat);

//	Model can("../objects/can.obj", can_mat);
	//BoundBox bb[10];
	int count = 0;
	for (auto it = modelMap.begin(); it != modelMap.end(); it++) {
		if ((it->first).substr(0, 6) == "ticket" || (it->first).substr(0, 2) == "id") {
			modelMap[it->first].rendered = false;
		}
	}

	// set models interactive type
	int i = 100;
	for (auto it = modelMap.begin(); it != modelMap.end(); it++, i++) {
		(it->second).code = i;
		std::string typeStr = (it->first).substr(0, 6);
		if (typeStr == "chairs") { (it->second).type = CHAIR; }
		else if (typeStr == "person") {
			(it->second).type = PERSON;
			modelMap[it->first].linkedID = "id" + (it->first).substr(6);
			modelMap[it->first].linkedTicket = "ticket" + (it->first).substr(6);
		}
		else if (typeStr == "can")
		{
			(it->second).type = CAN;
		}
		else { (it->second).type = NONE; }
	}

	// load bounding boxes
	BoundBox bb[10] = {
				BoundBox(modelMap["chairs1"].model.getMaxCords(), modelMap["chairs1"].model.getMinCords()),
				BoundBox(modelMap["chairs2"].model.getMaxCords(), modelMap["chairs2"].model.getMinCords()),
				BoundBox(modelMap["chairs3"].model.getMaxCords(), modelMap["chairs3"].model.getMinCords()),
						BoundBox(modelMap["chairs4"].model.getMaxCords(), modelMap["chairs4"].model.getMinCords()),
						BoundBox(modelMap["person1"].model.getMaxCords(), modelMap["person1"].model.getMinCords()),
						BoundBox(modelMap["person2"].model.getMaxCords(), modelMap["person2"].model.getMinCords()),
						BoundBox(modelMap["ticket1"].model.getMaxCords(), modelMap["ticket1"].model.getMinCords()),
						BoundBox(modelMap["ticket2"].model.getMaxCords(), modelMap["ticket2"].model.getMinCords()),
						BoundBox(modelMap["id1"].model.getMaxCords(), modelMap["id1"].model.getMinCords()),
						BoundBox(modelMap["id2"].model.getMaxCords(), modelMap["id2"].model.getMinCords())
	};

	BoundBox areaMap(glm::vec3(7.75f, 2.0f, -1.2f), glm::vec3(-7.75f, -2.0f, -7.8f));
	//	BoundBox areaMap(glm::vec3(20.75f, 2.0f, 20.2f), glm::vec3(-20.75f, -2.0f, -20.8f));
	//	BoundBox bb(ourModel.getMaxCords(), ourModel.getMinCords());
		// don't forget to enable shader before setting uniforms
	ourShader.use();
	skyboxShader.use();
	ourShader.setInt("texture1", 0);
	skyboxShader.setInt("skybox", 0);

	unsigned char res[4] = { 0,0,0,0 };
	GLint viewport[4];

	glm::vec4 origColour = glm::vec4(0.9f, 0.95f, 1.0f, 0.95f);
	Light lights[4];
	int RANGE = 15;
	for (int i = 0; i < 2; ++i) {
		lights[i].colour = origColour;
	}
	lights[0].position = glm::vec4(rand() * -RANGE, 0.0f, 3.0f, 1.0f);
	lights[1].position = glm::vec4(rand() * -RANGE, 0.0f, 3.0f, 1.0f);
	lights[2].position = glm::vec4(1.0f, 0.0f, 3.0f, 1.0f);
	lights[3].position = glm::vec4(-1.0f, 0.0f, 3.0f, 1.0f);
	int numFrames = 0;
	glm::vec3 camSave;

	double velocity = 0.1;

	glm::vec3 position(0, modelMap["can"].model.getMinCords().y, 0);
	float floor = modelMap["can"].model.getMinCords().y;
//	glm::mat4 IDMat;
//	can_mat = glm::translate(IDMat, glm::vec3(0.0f, 100.0f, -10.0f));

	// Text 2D
	//Text2D textRenderer = Text2D("text2d-font.dds");

	// Effects

	motionBlur = new MotionBlur(SCR_WIDTH, SCR_HEIGHT, false);
	motionBlur->setup();

	godRays = new GodRays(SCR_WIDTH, SCR_HEIGHT, false);
	godRays->setup();

	grayscale = new Grayscale(SCR_WIDTH, SCR_HEIGHT, false);
	grayscale->setup();

	inversion = new Inversion(SCR_WIDTH, SCR_HEIGHT, false);
	inversion->setup();
	
	std::vector<int> interestingKeys = std::vector<int>({
		GLFW_KEY_ESCAPE,
		GLFW_KEY_LEFT_CONTROL,
		GLFW_KEY_LEFT_SHIFT,
		GLFW_KEY_W,
		GLFW_KEY_S,
		GLFW_KEY_A,
		GLFW_KEY_D,
		GLFW_KEY_F1,
		GLFW_KEY_F2,
		GLFW_KEY_F3,
		GLFW_KEY_F4,
	});

	Keys keys = Keys(window, interestingKeys);
	
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
		processInput(window, areaMap, bb, sizeof(bb) / sizeof(bb[0]), keys);

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		glUniform4fv(glGetUniformLocation(ourShader.ID, "light_position[0]"), 1, glm::value_ptr(lights[0].position));
		glUniform4fv(glGetUniformLocation(ourShader.ID, "light_position[1]"), 1, glm::value_ptr(lights[1].position));
		glUniform4fv(glGetUniformLocation(ourShader.ID, "light_position[2]"), 1, glm::value_ptr(lights[2].position));
		glUniform4fv(glGetUniformLocation(ourShader.ID, "light_position[3]"), 1, glm::value_ptr(lights[3].position));
		glUniform4fv(glGetUniformLocation(ourShader.ID, "light_colour[0]"), 1, glm::value_ptr(lights[0].colour));
		glUniform4fv(glGetUniformLocation(ourShader.ID, "light_colour[1]"), 1, glm::value_ptr(lights[1].colour));
		glUniform4fv(glGetUniformLocation(ourShader.ID, "light_colour[2]"), 1, glm::value_ptr(lights[2].colour));
		glUniform4fv(glGetUniformLocation(ourShader.ID, "light_colour[3]"), 1, glm::value_ptr(lights[3].colour));

		ourShader.use();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);
		position.y += yupdate;
		yupdate = 0;
//GRAVITY TESTING CODE

		if (moveFlag == false && cany>-2.5)
		{
		//	std::cout << position.y << std::endl;

			cany -= (velocity*deltaTime) + (0.5*9.8*deltaTime*deltaTime);
			velocity += (9.8*deltaTime);

			glm::mat4 camMove;// = modelMap[moveModel].transform;

			camMove = glm::translate(camMove, glm::vec3(canx, cany, canz));
			camMove = glm::scale(camMove, glm::vec3(0.1f, 0.1f, 0.1f));
			modelMap["can"].model.t = camMove;
			modelMap["can"].transform = camMove;
		}
		else if (moveFlag == true)
		{
			velocity = 0.1;
		}
		

//END TEST

		constructScene(ourShader);

	//	std::cout << position.y << std::endl;

		if (flag == true)
		{
			glm::vec3 toPerson = glm::vec3(modelMap[focusPerson].transform[3]) - camera.Position;
			if (glm::distance(camera.Position, glm::vec3(modelMap[focusPerson].transform[3])) > 4) {
				camera.Position = glm::vec3(modelMap[focusPerson].transform[3]) - glm::vec3(0.99 * toPerson.x, 0.99 * toPerson.y, 0.99 * toPerson.z);
			}
		}
		else {
			camSave = camera.Position;
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

		motionBlur->render(view, projection);
		godRays->render(view, projection);
		grayscale->render(view, projection);
		inversion->render(view, projection);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------

		keys.update();
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
void processInput(GLFWwindow *window, BoundBox areaMap, BoundBox bb[], int arrLength, Keys keys)
{
	if (keys.isPressed(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && flag) {
		std::string idStr = modelMap[modelMap[focusPerson].linkedID].idName;
		std::string ticketStr = modelMap[modelMap[focusPerson].linkedTicket].ticketName;
		std::cout << idStr.substr(6, 9) << std::endl;
		if (idStr.substr(6, 9) == "_inval" || ticketStr.substr(6, 9) == "_inval"
			|| (idStr.substr(0, 6) != ticketStr.substr(0, 6))) {
			score--;
			std::cout << "Score (-1): " << score << std::endl;
		}
		else {
			score++;
			std::cout << "Score (+1): " << score << std::endl;
		}
		
		modelMap[modelMap[focusPerson].linkedID].rendered = false;
		modelMap[modelMap[focusPerson].linkedTicket].rendered = false;
		flag = false;
	}

	if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS && flag) {
		std::string idStr = modelMap[modelMap[focusPerson].linkedID].idName;
		std::string ticketStr = modelMap[modelMap[focusPerson].linkedTicket].ticketName;
		
		if (idStr.substr(6, 9) == "_valid" || ticketStr.substr(6, 9) == "_valid"
			|| (idStr.substr(0, 6) != ticketStr.substr(0, 6))) {
			score--;
			std::cout << "Score (-1): " << score << std::endl;
		}
		else {
			score++;
			std::cout << "Score (+1): " << score << std::endl;
		}
		
		modelMap[focusPerson].rendered = false;
		modelMap[modelMap[focusPerson].linkedID].rendered = false;
		modelMap[modelMap[focusPerson].linkedTicket].rendered = false;
		flag = false;
	}

	// Effects

	if (keys.isJustPressed(GLFW_KEY_F1)) {
		motionBlur->toggle();
		std::cout << "Motion blur toggled." << std::endl;
	}

	if (keys.isJustPressed(GLFW_KEY_F2)) {
		godRays->toggle();
		std::cout << "God rays toggled." << std::endl;
	}

	if (keys.isJustPressed(GLFW_KEY_F3)) {
		grayscale->toggle();
		std::cout << "Grayscale toggled." << std::endl;
	}

	if (keys.isJustPressed(GLFW_KEY_F4)) {
		inversion->toggle();
		std::cout << "Colour inversion toggled." << std::endl;
	}

	// Movement

	int speed = 1;

	if (keys.isPressed(GLFW_KEY_LEFT_SHIFT)) {
		speed = 2;
	}

	if (keys.isPressed(GLFW_KEY_LEFT_CONTROL)) {
		camera.setCrouch(true);
	}

	if (keys.isReleased(GLFW_KEY_LEFT_CONTROL)) {
		camera.setCrouch(false);
	}

	if (keys.isPressed(GLFW_KEY_W)) {
		camera.ProcessKeyboard(FORWARD, deltaTime * speed, areaMap, bb, arrLength, flag);
	}

	if (keys.isPressed(GLFW_KEY_S)) {
		camera.ProcessKeyboard(BACKWARD, deltaTime * speed, areaMap, bb, arrLength, flag);
	}
	
	if (keys.isPressed(GLFW_KEY_A)) {
		camera.ProcessKeyboard(LEFT, deltaTime * speed, areaMap, bb, arrLength, flag);
	}

	if (keys.isPressed(GLFW_KEY_D)) {
		camera.ProcessKeyboard(RIGHT, deltaTime * speed, areaMap, bb, arrLength, flag);
	}
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

			//std::cout << (int)res[0] << std::endl;
			std::string modelStr;
			for (auto it = modelMap.begin(); it != modelMap.end(); it++) {
				if ((it->second).code == *res) {
					modelStr = it->first;
				}
			}
			if (glm::distance(camera.Position, glm::vec3(modelMap[modelStr].transform[3])) < 7.0f && moveFlag == false) {
				switch (modelMap[modelStr].type) {
				case NONE:
					std::cout << modelMap[modelStr].code << ": Pretty, but uninteractable." << std::endl;
					break;
				case PERSON:
					std::cout << modelMap[modelStr].code << ": How's it going?" << std::endl;
					focusPerson = modelStr;
					flag = true;
					modelMap[modelMap[focusPerson].linkedID].rendered = true;
					modelMap[modelMap[focusPerson].linkedTicket].rendered = true;
					code = modelMap[modelStr].code;
					break;
				case CHAIR:
					std::cout << modelMap[modelStr].code << ": I'll sit down." << std::endl;

					camera.Position = glm::vec3((modelMap[modelStr].transform)[3]);
					camera.Position[1] = -0.5;
					camera.setSitting();

					break;

				case CAN:
					std::cout << modelMap[modelStr].code << ": Can." << std::endl;
					moveFlag = true;
					moveModel = modelStr;
				//	std::cout << moveModel << std::endl;
					reposx = 0;
					reposy = 0;
					break;
				}
			}
		}
		else
		{
			flag = false;
			modelMap[modelMap[focusPerson].linkedID].rendered = false;
			modelMap[modelMap[focusPerson].linkedTicket].rendered = false;
			//focusPerson = "";
		}

	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		moveFlag = false;
		moveModel = "";
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
	if (moveFlag == true)
	{
		if (currX > lastX)
		{
			reposx = 0.05;
		}
		else if (currX < lastX)
		{
			reposx = -0.05;
		}
		else
		{
			reposx = 0;
		}

		if (currY > lastY)
		{
			reposy = -0.025;
		}
		else if (currY < lastY)
		{
			reposy = 0.025;
		}
		else
		{
			reposy = 0;
		}
	}

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

	if (moveFlag == true)
	{
		yupdate = reposy;
		glm::mat4 camMove = modelMap[moveModel].transform;

		camMove = glm::translate(camMove, glm::vec3(reposx, reposy, 0));
		canx += reposx/10;
		cany += reposy/10;
		modelMap[moveModel].transform = camMove;
		modelMap[moveModel].model.t = camMove;	//not sure what t is
		reposx = 0;
		reposy = 0;
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

