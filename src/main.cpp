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
	#include "shader.h"

    //Define an error callback  
    static void error_callback(int error, const char* description)  
    {  
        fputs(description, stderr);  
        _fgetchar();  
    }  
      
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

        //==================================
        //        Load vertex data
        //==================================

        //create Vertex array object
        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Example: generate vertex buffers
        GLuint buffer;
        glGenBuffers(1, &buffer);

        //load cube model (note this is drawn without using an element buffer)
       GLfloat vertices[] = {
            -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
             0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

             0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
             0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
             0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
             0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
             0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
             0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
             0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
             0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f
        };

        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

        //==================================
        //     Compile and Link Shaders
        //==================================

		Shader shader = Shader("shader.vert", "shader.frag");
		shader.bind();

        //==================================
        //    Link Vertex Data to Shaders
        //==================================

        //link vertex data to shader
		GLint posAttrib = shader.getAttribLocation("position");
        glVertexAttribPointer(posAttrib,                //buffer identifier
                              3,                        //How many data points to read
                              GL_FLOAT,                 //Data type
                              GL_FALSE,                 //Whether or not to clamp values between -1,1
                              8 * sizeof(float),        //Stride (byte offset between consecutive values)
                              0                         //pointer to first attribute (see below for better examples)
        );
        glEnableVertexAttribArray(posAttrib);
        
		GLint colourAttrib = shader.getAttribLocation("colour");
        glVertexAttribPointer(colourAttrib, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(colourAttrib);

		GLint textureAttrib = shader.getAttribLocation("texcoord");
        glVertexAttribPointer(textureAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(textureAttrib);

        //==================================
        //          Load Texture
        //==================================

        //Create texture buffer:
        GLuint tex;
        glGenTextures(1,&tex);
        glBindTexture(GL_TEXTURE_2D,tex);

        //Load image
        int width, height;
        unsigned char* image =
            SOIL_load_image("kitten.png", &width, &height, 0, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                      GL_UNSIGNED_BYTE, image);
        SOIL_free_image_data(image);
        
        //Set sampler parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       

        //==================================
        //              Main Loop
        //==================================

        //Set a background color  
        glClearColor(0.0f, 0.0f, 1.0f, 0.0f); 

        //TODO: turn on depth buffer
        glEnable(GL_DEPTH_TEST); 
      
        //Main Loop  
        clock_t start = std::clock();
        do  
        {  
            double frame_time = (double) (clock()-start) / double(CLOCKS_PER_SEC);
            //==================================
            //       TODO: 3D Transforms
            //==================================

            //TODO: create model matrix based on time
            glm::mat4 model;
            float period = 5; //seconds
            model = glm::rotate(model, 360 * float(frame_time) / period , glm::vec3(0.0f, 0.0f, 1.0f));
            
    		//TODO: load model matrix into shader      
            GLint uniModel = shader.getUniformLocation("model");
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
            
            //TODO: create and load view matrix
            glm::mat4 view = glm::lookAt(
                glm::vec3(1.2f, 1.2f, 1.2f), //Camera position
                glm::vec3(0.0f, 0.0f, 0.0f), //Camera view target
                glm::vec3(0.0f, 0.0f, 1.0f)  //Camera up vector which is usually z
            );
            GLint uniView = shader.getUniformLocation("view");
            glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

            //TODO: create and load projection matrix
            glm::mat4 proj = glm::perspective(45.0f,                              //VERTICAL FOV
                                              float(window_width) / float(window_height),       //aspect ratio
                                              1.0f,                                             //near plane distance (min z)
                                              10.0f                                             //Far plane distance (max z)
            );
            GLint uniProj = shader.getUniformLocation("proj");
            glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

            //==================================
            //          Draw Model
            //==================================

            //Clear color buffer  
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
            
            glDrawArrays(GL_TRIANGLES, 0, 36); 
            // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); 
      
            //Swap buffers  
            glfwSwapBuffers(window);  
            //Get and organize events, like keyboard and mouse input, window resizing, etc...  
            glfwPollEvents();  
        
        } //Check if the ESC key had been pressed or if the window had been closed  
        while (!glfwWindowShouldClose(window));  
      
        //Close OpenGL window and terminate GLFW  
        glfwDestroyWindow(window);  
        //Finalize and clean up GLFW  
        glfwTerminate();  
      
        exit(EXIT_SUCCESS);  
    }  