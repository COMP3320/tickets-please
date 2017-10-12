#ifndef LIGHT_H
#define LIGHT_H

#include <GL/glew.h>
#include "glm/glm.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Light
{
public:
	unsigned int ID;
	glm::vec3 position;
	glm::vec3 intensities; //a.k.a. the color of the light
	float attenuation;
	float ambientCoefficient;

	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------
	Light(glm::vec3 pos,
		glm::vec3 inten, //a.k.a. the color of the light
		float atten,
		float ambCoeff);

	void setPosition(glm::vec3 pos);
};

#endif