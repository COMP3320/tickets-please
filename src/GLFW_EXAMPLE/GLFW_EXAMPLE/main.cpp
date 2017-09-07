	//Include GLEW  
    //#define GLEW_STATIC

	//Library for loading textures (Simple OpenGL Image Library)
	#include <SOIL.h>

    #include <GL/glew.h>  

	#include<iostream> //cout
    #include <fstream> //fstream
	#include <ctime> 

    //Include GLFW  
    #include <GLFW/glfw3.h>  
      
    //Include the standard C++ headers  
    #include <stdio.h>  
    #include <stdlib.h> 

    //Include matrix libraries
    #include "glm/glm.hpp"
    #include "glm/gtc/matrix_transform.hpp"
    #include "glm/gtc/type_ptr.hpp"

	//Include personal libraries
	#include "Shader.h"
	#include "Model.h"
	#include "Camera.h"

    //Define an error callback  
    static void error_callback(int error, const char* description){  
        fputs(description, stderr);  
        _fgetchar();  
    }  

	// settings
	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;

	// camera
	Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
	float lastX = SCR_WIDTH / 2.0f;
	float lastY = SCR_HEIGHT / 2.0f;
	bool firstMouse = false;

	// timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void processInput(GLFWwindow *window);

    //Define the key input callback  
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)  
    {  
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)  
        glfwSetWindowShouldClose(window, GL_TRUE);  
    } 

    int main( void )  
    {  
        //Set the error callback  
        glfwSetErrorCallback(error_callback);  

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        //Initialize GLFW  
        if (!glfwInit())  
        {  
            exit(EXIT_FAILURE);  
        }  
      
        //Set the GLFW window creation hints - these are optional  
        //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version  
        //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2); //Request a specific OpenGL version  
        //glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing  
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  
      
        //Declare a window object  
        GLFWwindow* window;  
      
        //Create a window and create its OpenGL context
        const int window_width = 640;
        const int window_height = 480;
        window = glfwCreateWindow(window_width, window_height, "Test Window", NULL, NULL);  
      
        //If the window couldn't be created  
        if (!window)  
        {  
            fprintf( stderr, "Failed to open GLFW window.\n" );  
            glfwTerminate();  
            exit(EXIT_FAILURE);  
        }  
      
        //This function makes the context of the specified window current on the calling thread.   
        glfwMakeContextCurrent(window);  
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

		// tell GLFW to capture our mouse
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        //Sets the key callback  
        glfwSetKeyCallback(window, key_callback);  
      
        //Initialize GLEW  
		glewExperimental = GL_TRUE;
        GLenum err = glewInit();  
      
        //If GLEW hasn't initialized  
        if (err != GLEW_OK)   
        {  
            fprintf(stderr, "Error: %s\n", glewGetErrorString(err));  
            return -1;  
        }

		glEnable(GL_DEPTH_TEST);

        //==================================
        //     Compile and Link Shaders
        //==================================

		Shader shader = Shader("shader.vert", "shader.frag");

		Model ourModel("../nanosuit/nanosuit.obj");       

        //==================================
        //              Main Loop
        //==================================

        //Set a background color  
		
		while (!glfwWindowShouldClose(window)) {
			float currFrame = float(glfwGetTime());
			deltaTime = currFrame - lastFrame;
			lastFrame = currFrame;

			processInput(window);

			glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			shader.bind();

			//TODO: create and load projection matrix
			glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			GLint uniProj = shader.getUniformLocation("proj");
			glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

			glm::mat4 view = camera.GetViewMatrix();
			GLint uniView = shader.getUniformLocation("view");
			glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

			glm::mat4 model;
			model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
			shader.getUniformLocation("model");
			ourModel.Draw(shader);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
      
        //Close OpenGL window and terminate GLFW  
        glfwDestroyWindow(window);  
        //Finalize and clean up GLFW  
        glfwTerminate();  
      
        exit(EXIT_SUCCESS);  
    }  

	// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
	// ---------------------------------------------------------------------------------------------------------
	void processInput(GLFWwindow *window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);
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