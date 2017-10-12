#include "game.h"

//
// Constructors 
//

Game::Game(int width, int height) {
	this->width = width;
	this->height = height;

	lastX = width / 2.0f;
	lastY = height / 2.0f;

	firstMouse = true;
	wasEscapePressed = false;
}

// ------------------------------------

//
// Actions 
//

bool Game::setup() {
	// Check that we initialised GLFW successfully (created a window)
	if (!setupGLFW()) {
		return false;
	}

	//Check that we initialised GLEW successfully (loaded all OpenGL function pointers)
	if (!setupGLEW()) {
		return false;
	}

	//setupCamera();
	setupMenu();
	//setupCubemap();
	//setupState();
	//setupSkybox();
	//setupShaders();

	glfwSwapInterval(1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 640.0, 480.0, 0.0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// load models
	// -----------

	/*ourModel3 = new Model("resources/objects/map2.obj");
	std::cout << "Loaded ourModel3" << std::endl;
	ourModel4 = new Model("resources/objects/map2.obj");
	std::cout << "Loaded ourModel4" << std::endl;
	ourModel5 = new Model("resources/objects/mapend.obj");
	std::cout << "Loaded ourModel5" << std::endl;
	ourModel6 = new Model("resources/objects/mapend.obj");
	std::cout << "Loaded ourModel6" << std::endl;*/

	return true;
}

int Game::run() {
	// Run the main game loop
	loop();

	// When the loop ends, cleanup any resources used
	cleanup();

	// Return successfully
	return 0;
}

// ------------------------------------

//
// Getters 
//

GLFWwindow* Game::getWindow() {
	return this->window;
}

// ------------------------------------

//
// Callbacks 
//

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void Game::onFrameBufferResize(int width, int height)
{
	std::cout << "onFrameBufferResize called" << std::endl << std::endl;

	this->width = width;
	this->height = height;

	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Game::onMouseMoved(double xPosition, double yPosition)
{
	std::cout << "onMouseMoved called" << std::endl << std::endl;

	if (firstMouse)
	{
		lastX = xPosition;
		lastY = yPosition;
		firstMouse = false;
	}

	float xOffset = xPosition - lastX;
	float yOffset = lastY - yPosition; // reversed since y-coordinates go from bottom to top

	lastX = xPosition;
	lastY = yPosition;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Game::onMouseScrolled(double xOffset, double yOffset)
{
	std::cout << "onMouseScrolled called" << std::endl << std::endl;

	camera.ProcessMouseScroll(yOffset);
}

// ------------------------------------

//
// Private methods
//

bool Game::setupGLFW() {
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	window = glfwCreateWindow(width, height, "Tickets, Please", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;

		glfwTerminate();

		return false;
	}

	glfwMakeContextCurrent(window);

	return true;
}

bool Game::setupGLEW() {
	glewExperimental = GL_TRUE;

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	GLenum err = glewInit();

	// If GLEW hasn't initialized  
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

		return false;
	}

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	return true;
}

void Game::setupCamera() {
	// Initialise camera
	camera = Camera(glm::vec3(0.0f, 0.0f, -4.0f));
}

void Game::setupMenu() {
	// Initialise menu
	menu = new Menu(window);
}

void Game::setupCubemap() {
	// get cubemap texture
	std::vector<std::string> faces = {
		"resources/objects/skybox/right.tga",
		"resources/objects/skybox/left.tga",
		"resources/objects/skybox/top.tga",
		"resources/objects/skybox/bottom.tga",
		"resources/objects/skybox/back.tga",
		"resources/objects/skybox/front.tga"
	};

	cubemapTexture = loadCubemap(faces);
}

void Game::setupState() {
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Game::setupSkybox() {
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void Game::setupShaders() {
	// build and compile shaders
	// don't forget to enable shader before setting uniforms
	// -------------------------
	shader = new Shader("resources/shaders/default/shader.vert", "resources/shaders/default/shader.frag");

	shader->use();
	shader->setInt("texture1", 0);

	skyboxShader = new Shader("resources/shaders/cubemap/shader.vert", "resources/shaders/cubemap/shader.frag");

	skyboxShader->use();
	skyboxShader->setInt("skybox", 0);
}

void Game::updateTime() {
	// per-frame time logic
	// --------------------
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void Game::cleanup() {
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVAO);

	glfwTerminate();
}

void Game::loop() {
	while (!glfwWindowShouldClose(window))
	{
		updateTime();
		processInput();
		render();
	}
}

void Game::render() {
	prepareOutput();
	sendOutput();
}

void Game::prepareOutput() {
	if (menu->isVisible()) {
		// Tell GLFW to release our mouse
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		//glDisable(GL_DEPTH_TEST);

		menu->render();
	}

	else {
		// Tell GLFW to capture our mouse
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glEnable(GL_DEPTH_TEST);

		glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader->use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader->setMat4("projection", projection);
		shader->setMat4("view", view);

		glm::mat4 model3;
		//	model3 = glm::rotate(model3, 1.5f, glm::vec3(0.0f, 0.0f, 0.0f));
		model3 = glm::translate(model3, glm::vec3(-0.75f, -1.5f, 0.0f)); // translate it down so it's at the center of the scene
		model3 = glm::scale(model3, glm::vec3(0.65f, 0.50f, 0.50f));	// it's a bit too big for our scene, so scale it down

		shader->setMat4("model", model3);
		ourModel3->Draw(*shader);

		glm::mat4 model4;
		model4 = glm::translate(model4, glm::vec3(-0.75f, -1.5f, -5.38f)); // translate it down so it's at the center of the scene
		model4 = glm::scale(model4, glm::vec3(0.65f, 0.50f, 0.50f));
		shader->setMat4("model", model4);
		ourModel4->Draw(*shader);

		glm::mat4 model5;
		model5 = glm::translate(model5, glm::vec3(0.05f, -1.5f, -9.38f)); // translate it down so it's at the center of the scene
		model5 = glm::scale(model5, glm::vec3(0.65f, 0.50f, 0.50f));
		shader->setMat4("model", model5);
		ourModel5->Draw(*shader);

		glm::mat4 model6;
		model6 = glm::rotate(model6, 3.15f, glm::vec3(0.0f, 1.0f, 0.0f));
		model6 = glm::translate(model6, glm::vec3(0.05f, -1.5f, -2.0f)); // translate it down so it's at the center of the scene
		model6 = glm::scale(model6, glm::vec3(0.65f, 0.50f, 0.50f));
		shader->setMat4("model", model6);
		ourModel6->Draw(*shader);

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader->use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader->setMat4("view", view);
		skyboxShader->setMat4("projection", projection);

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default
	}
}

void Game::sendOutput() {
	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
}

unsigned int Game::loadCubemap(std::vector<std::string> faces) {
	unsigned int textureId;

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

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

	return textureId;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Game::processInput()
{
	// Escape toggles the menu displaying
	switch (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
	case GLFW_PRESS: {
		// But Shift + Escape will immediately close the window
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}

		wasEscapePressed = true;

		break;
	}

	case GLFW_RELEASE: {
		if (wasEscapePressed) {
			menu->toggleVisibility();
		}

		wasEscapePressed = false;

		break;
	}
	}

	if (!menu->isVisible()) {
		float speed = 1.0f;

		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			speed = 2.0f;
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
			// Slow the player down if they are crouching
			speed *= 0.5f;

			camera.setCrouch(true);
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE) {
			camera.setCrouch(false);
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.ProcessKeyboard(FORWARD, deltaTime * speed);
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.ProcessKeyboard(BACKWARD, deltaTime * speed);
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera.ProcessKeyboard(LEFT, deltaTime * speed);
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.ProcessKeyboard(RIGHT, deltaTime * speed);
		}
	}
}