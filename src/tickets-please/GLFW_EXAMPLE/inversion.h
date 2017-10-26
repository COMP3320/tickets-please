

#ifndef INVERSION_H
#define INVERSION_H

#include <GL/glew.h>
#include "glm/glm.hpp"
#include <iostream>

#include "shader.h"

class Inversion {
public:
	/*  Functions  */
	// constructor
	Inversion(int width, int height, bool enabled = false)
	{
		this->width = width;
		this->height = height;
		this->enabled = enabled;
		this->shader = new Shader("inversion.vert", "inversion.frag");
	}

	void setup() {
		// Process quad VAO/VBO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		// Configure frame buffer
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		// Multisampling!
		// 1. Configure multisample framebuffer
		glGenTextures(1, &framebuffer);
		glBindTexture(GL_FRAMEBUFFER, framebuffer);

		// 2. Create multisampled colour attachment tex
		glGenTextures(1, &texColBuffMS);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texColBuffMS);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MS_SIZE, GL_RGB, width, height, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texColBuffMS, 0);

		// 3. Create render buffer for depth & stencil
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, MS_SIZE, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "ERROR: Framebuffer is not complete!" << std::endl;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 4. Create intermediate framebuffer
		glGenFramebuffers(1, &intermediateFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

		// Current frame buffer
		glGenTextures(1, &texColorBuffer);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

		if (enabled == false) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}

	void render(glm::mat4 view, glm::mat4 projection) {
		if (enabled) {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			// Now back to default framebuffer and draw our quad
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			shader->use();
			shader->setMat4("view", view);
			shader->setMat4("proj", projection);

			glBindVertexArray(quadVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texColorBuffer);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glEnable(GL_DEPTH_TEST);
		}
	}

	void enable() {
		this->enabled = true;
	}

	void disable() {
		this->enabled = false;
	}

	void toggle() {
		this->enabled = !this->enabled;
	}

private:
	int width;
	int height;
	bool enabled = false;

	Shader* shader;
	unsigned int quadVAO;
	unsigned int quadVBO;
	unsigned int fbo;
	unsigned int framebuffer;
	int MS_SIZE = 8;
	unsigned int texColBuffMS;
	unsigned int rbo;
	unsigned int intermediateFBO;
	unsigned int texColorBuffer;

	// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// positions   // texCoords
	float quadVertices[24] = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};
};
#endif

