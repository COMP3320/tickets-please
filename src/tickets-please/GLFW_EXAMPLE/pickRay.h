
#ifndef RAY_H
#define RAY_H
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>

class PickRay
{
	public:
		PickRay(glm::vec3 temp1, glm::vec3 temp2)
		{
			posInWorld = temp1;
			direction = temp2;
		}

		void intersectionXY(float* worldPos)
		{
			float s = -posInWorld.z / direction.z;
			worldPos[0] = posInWorld.x + direction.x*s;
			worldPos[1] = posInWorld.y + direction.y*s;
			worldPos[2] = 0;
		}

		glm::vec3 getPosInWorld()
		{
			return posInWorld;
		}

		glm::vec3 getDirection()
		{
			return direction;
		}
	private:
		glm::vec3 posInWorld;
		glm::vec3 direction;
};
#endif