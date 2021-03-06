#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include "stb_image.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

class Camera
{
private:
	float cameraSpeed = 15.0f;
	bool firstMouse = true;
	float width, height;
	float lastx = 0.0f, lasty = 0.0f;
	float deltaTime = 0.0f;
	const float sensitivity = 0.05f;
	float yaw = -90.0f;
	float pitch = 0.0f;
	const glm::vec3 gravity;
	glm::vec3 speed = glm::vec3(0.0f, 0.0f, 0.0f);
public:
	bool useGravity = false;
	bool drawCube = false;
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 100.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	Camera(float width, float height, const glm::vec3 gravityForce): 
		width(width), height(height), gravity(gravityForce), lastx(width/2), lasty(height/2) {}
	Camera() {}

	glm::mat4 CreateViewMatrix() {
		return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}
	glm::mat4 CreateProjectionMatix(float &radians, float &width, float &height) {
		return glm::perspective(glm::radians(radians), width / height, 0.1f, 400.0f);
	}

	void MoveCameraFunction(GLFWwindow* window, float deltaTime){
		this->deltaTime = deltaTime;
		if (useGravity) {
			SimulateGravity();
			UseCameraWithGravity(window);
		}
		else {
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				cameraPos += cameraSpeed * deltaTime * cameraFront;
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				cameraPos -= cameraSpeed * deltaTime * cameraFront;
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				cameraPos -= cameraSpeed * deltaTime * glm::normalize(glm::cross(cameraFront, cameraUp));
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				cameraPos += cameraSpeed * deltaTime * glm::normalize(glm::cross(cameraFront, cameraUp));
		}
	}

	void SimulateGravity() {
		if (cameraPos.y > 0) {
			speed += deltaTime * gravity;
			cameraPos += speed;
		}
		else {
			speed = glm::vec3(0.0f);
			cameraPos.y = 0.0f;
		}
	}

	void UseCameraWithGravity(GLFWwindow* window) {
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			cameraSpeed = 45.0f;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
			cameraSpeed = 25.0f;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS and cameraPos.y == 0.0f) {
			speed = glm::vec3(0.0f, 0.10f, 0.0f);
			cameraPos += speed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			cameraPos.x += cameraSpeed * deltaTime * cameraFront.x;
			cameraPos.z += cameraSpeed * deltaTime * cameraFront.z;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			cameraPos.x -= cameraSpeed * deltaTime * cameraFront.x;
			cameraPos.z -= cameraSpeed * deltaTime * cameraFront.z;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			cameraPos -= cameraSpeed * deltaTime * glm::normalize(glm::cross(cameraFront, cameraUp));
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			cameraPos += cameraSpeed * deltaTime * glm::normalize(glm::cross(cameraFront, cameraUp));
	}

	void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
		if (firstMouse) {
			lastx = xpos;
			lasty = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastx;
		float yoffset = lasty - ypos;
		lastx = xpos;
		lasty = ypos;

		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 direction;

		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);
	}


};