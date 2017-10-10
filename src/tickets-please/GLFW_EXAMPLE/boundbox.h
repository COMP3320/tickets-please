#ifndef BOUND_H
#define BOUND
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <math.h>

#include <vector>

class BoundBox
{
public:
	BoundBox(glm::vec3 maxCords, glm::vec3 minCords)
	{
		xdist = maxCords.x-minCords.x;
		ydist = maxCords.y-minCords.y;
		zdist = maxCords.z-minCords.z;

		size = glm::vec3(xdist, ydist, zdist);
		center = glm::vec3((maxCords.x+minCords.x)/2, (maxCords.y + minCords.y)/2, (maxCords.z + minCords.z)/2);
	}

	glm:: vec3 getSize()
	{
		return size;
	}

	glm::vec3 getCenter()
	{
		return center;
	}

	void updateSize(glm::vec3 newSize)
	{
		size = newSize;
	}

	void updateCenter(glm::vec3 newCenter)
	{
		center = newCenter;
	}

private:
	float xdist, ydist, zdist;
	glm::vec3 center, size;
};
#endif