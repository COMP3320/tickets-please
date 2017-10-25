
#ifndef BOUND_H
#define BOUND_H
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <math.h>

#include <vector>
using namespace std;
class BoundBox
{
public:
	BoundBox() {}
	BoundBox(glm::vec3 maxCords, glm::vec3 minCords)
	{
		xdist = maxCords.x-minCords.x;
		ydist = maxCords.y-minCords.y;
		zdist = maxCords.z-minCords.z;
		max = maxCords;
		min = minCords;
		maxo = max;
		mino = min;
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

	glm::vec3 getMax()
	{
		return max;
	}

	glm::vec3 getMin()
	{
		return min;
	}

	void scale(glm::vec3 newScale)
	{
		max = glm::vec3(maxo.x*newScale.x, maxo.y*newScale.y, maxo.z*newScale.z);
		min = glm::vec3(mino.x*newScale.x, mino.y*newScale.y, mino.z*newScale.z);
//		std::cout << "SCALE x " << max.x << " y " << max.y << " z " << max.z << std::endl;
//		std::cout << "SCALE x " << min.x << " y " << min.y << " z " << min.z << std::endl;
	}

	void translate(glm::vec3 newScale)
	{
		max = glm::vec3(maxo.x+newScale.x, maxo.y+newScale.y, maxo.z+newScale.z);
		min = glm::vec3(mino.x+newScale.x, mino.y+newScale.y, mino.z+newScale.z);

//		std::cout << "TRANS x " << max.x << " y " << max.y << " z " << max.z << std::endl;
//		std::cout << "TRANS x " << min.x << " y " << min.y << " z " << min.z << std::endl;
	}

private:
	float xdist, ydist, zdist;
	glm::vec3 center, size, max, min, maxo, mino;
};
#endif