#include "light.h"


Light::Light(
	glm::vec3 pos,
	glm::vec3 inten, //a.k.a. the color of the light
	float atten,
	float ambCoeff
	)
{
	
}

void Light::setPosition(glm::vec3 pos)
{
	position = pos;
}