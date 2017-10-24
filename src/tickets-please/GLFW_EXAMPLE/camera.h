

#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "boundbox.h"
#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	CROUCH
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVTY = 0.05f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Eular Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;
	int test = 0;
	bool isCrouched, isSitting;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		isCrouched = false;
		updateCameraVectors();
	}
	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime, BoundBox areaMap, BoundBox bb[], int arrLength)
	{	// && Position.x >= (bb.getMin()).x >= Position.x && Position.x <= (bb.getMax()).x && Position.z >= (bb.getMin()).z && Position.z <= (bb.getMax()).z
		float velocity = MovementSpeed * deltaTime;
		glm::vec3 pos1, pos2, pos3, pos4;
		pos1 = Position + Front * velocity;
		pos2 = Position - Front * velocity;
		pos3 = Position - Right * velocity;
		pos4 = Position + Right * velocity;

		bool boundCheck = boundaryCheck(direction, areaMap, bb, arrLength, pos1, pos2, pos3, pos4);
//		std::cout << "Curr position x " << Position.x << "Curr position y " << Position.y << "Curr position z " << Position.z << std::endl;
//		std::cout << "Max x " << bb[1].getMax().x << "Min x " << bb[1].getMin().x << "Max y " << bb[1].getMax().y << "Min y " << bb[1].getMin().y << "Max z " << bb[1].getMax().z << "Min z " << bb[1].getMin().z << std::endl;
		if (direction == FORWARD	&& (boundCheck || isSitting))	{ Position = pos1; }
		if (direction == BACKWARD	&& (boundCheck || isSitting))	{ Position = pos2; }
		if (direction == LEFT		&& (boundCheck || isSitting))	{ Position = pos3; }
		if (direction == RIGHT		&& (boundCheck || isSitting))	{ Position = pos4; }
	}

	bool boundaryCheck(Camera_Movement direction, BoundBox areaMap, BoundBox bb[], int arrLength, glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4)
	{
		bool boundCheck = true;
		if (direction == FORWARD)
		{
			boundCheck = boundCheck && (pos1.x < areaMap.getMax().x && pos1.x > areaMap.getMin().x && pos1.z < areaMap.getMax().z && pos1.z > areaMap.getMin().z);
			for (int i = 0; i < arrLength; i++)
			{
				boundCheck = boundCheck && !(pos1.x < bb[i].getMax().x && pos1.x > bb[i].getMin().x && pos1.z < bb[i].getMax().z && pos1.z > bb[i].getMin().z);
			}
		}
		if (direction == BACKWARD)
		{
			boundCheck = boundCheck && (pos2.x < areaMap.getMax().x && pos2.x > areaMap.getMin().x && pos2.z < areaMap.getMax().z && pos2.z > areaMap.getMin().z);
			for (int i = 0; i < arrLength; i++)
			{
				boundCheck = boundCheck && !(pos2.x < bb[i].getMax().x && pos2.x > bb[i].getMin().x && pos2.z < bb[i].getMax().z && pos2.z > bb[i].getMin().z);
			}
		}
		if (direction == LEFT)
		{
			boundCheck = boundCheck && (pos3.x < areaMap.getMax().x && pos3.x > areaMap.getMin().x && pos3.z < areaMap.getMax().z && pos3.z > areaMap.getMin().z);
			for (int i = 0; i < arrLength; i++)
			{
				boundCheck = boundCheck && !(pos3.x < bb[i].getMax().x && pos3.x > bb[i].getMin().x && pos3.z < bb[i].getMax().z && pos3.z > bb[i].getMin().z);
			}
		}
		if (direction == RIGHT)
		{
			boundCheck = boundCheck && (pos4.x < areaMap.getMax().x && pos4.x > areaMap.getMin().x && pos4.z < areaMap.getMax().z && pos4.z > areaMap.getMin().z);
			for (int i = 0; i < arrLength; i++)
			{
				boundCheck = boundCheck && !(pos4.x < bb[i].getMax().x && pos4.x > bb[i].getMin().x && pos4.z < bb[i].getMax().z && pos4.z > bb[i].getMin().z);
			}
		}
		if (boundCheck == true && isSitting) { 
			isSitting = false; 
			Position[1] = 0.0f;
		}
		return boundCheck;
	}

	void setCrouch(bool setting) { Position[1] = ((setting == true || isSitting) ? -0.5f : 0.0f); }
	void setSitting() { isSitting = true; }

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Eular angles
		updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		if (Zoom >= 1.0f && Zoom <= 45.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 45.0f)
			Zoom = 45.0f;
	}

private:
	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
#endif

