#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include "stb_image.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <vector>
using namespace std;

class model3D {
private:
	Camera* cameraPointer;
	bool gravityEnabled = true;
	bool floorBound = false;
	bool wallBound = false;
	unsigned int buffer;
	unsigned int vertexBuffer;
	unsigned int myTempTexture;
	glm::vec3 modelSpeed = glm::vec3(0.0f);
	const glm::vec3 gravityForce;
	glm::vec3 frictionForce;
	glm::vec3 frictionSpeed = glm::vec3(0.0f);
	glm::vec3 gravitySpeed = glm::vec3(0.0f);
public:
	CreateShader* shaderPointer;
	vector <unsigned int> myTextures;
	glm::mat4 myModel;
	glm::vec3 myPosition;

	model3D(Camera* cameraPointer, CreateShader* shaderPointer, glm::vec3 gravityForce, glm::vec3 frictionForce) :
		cameraPointer(cameraPointer), shaderPointer(shaderPointer), gravityForce(gravityForce),
		frictionForce(frictionForce)
		{}

	model3D() {}

	void setVertexAndBuffer(float vertices[], unsigned int size, unsigned int typeID) {
		if (typeID == 2)
			typeID = 3;
		else
			typeID = 0;

		glGenVertexArrays(1, &vertexBuffer);
		glGenBuffers(1, &buffer);

		glBindVertexArray(vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (5 + typeID) * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (5 + typeID) * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		if (typeID == 3) {
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, (5 + typeID) * sizeof(float), (void*)(5 * sizeof(float)));
			glEnableVertexAttribArray(2);
		}
	}

	void loadTexture(char const* path) {
		glGenTextures(1, &myTempTexture);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data) {

			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, myTempTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}

		else {
			cout << "Failed to load texture. " << endl;
		}

		unsigned int tempID;
		tempID = myTempTexture;
		myTextures.push_back(tempID);
	}

	void DrawObject(unsigned int startingPoint, unsigned int endingPoint) {
		glBindVertexArray(vertexBuffer);
		glDrawArrays(GL_TRIANGLES, startingPoint, endingPoint);
	}

	void setMaterialProperties(int textureID, float shininess) {
		shaderPointer->setInt("material.diffuse", textureID);
		shaderPointer->setInt("material.specular", textureID);
		shaderPointer->setFloat("material.shininess", shininess);
	}

	void setMaterialProperties(int firstTextureID, int secondTextureID, float shininess) {
		shaderPointer->setInt("material.diffuse", firstTextureID);
		shaderPointer->setInt("material.specular", secondTextureID);
		shaderPointer->setFloat("material.shininess", shininess);
	}

	void setSpotLightProperties(glm::vec3 vecProperties[], float floatProperties[]) {
		shaderPointer->setVec3("spotLight.position", vecProperties[0]);
		shaderPointer->setVec3("spotLight.direction", vecProperties[1]);
		shaderPointer->setVec3("spotLight.color", vecProperties[2]);
		shaderPointer->setVec3("spotLight.ambient", vecProperties[3]);
		shaderPointer->setVec3("spotLight.diffuse", vecProperties[4]);
		shaderPointer->setVec3("spotLight.specular", vecProperties[5]);
		shaderPointer->setFloat("spotLight.constant", floatProperties[0]);
		shaderPointer->setFloat("spotLight.linear", floatProperties[1]);
		shaderPointer->setFloat("spotLight.quadratic", floatProperties[2]);
		shaderPointer->setFloat("spotLight.cutOff", floatProperties[3]);
		shaderPointer->setFloat("spotLight.outerCutOff", floatProperties[4]);
	}

	void setSpotLightProperties(glm::vec3 vecProperties[], float floatProperties[], unsigned int modelCount) {
		for (int i = 0; i < modelCount; i++) {
			string number = to_string(i);
			shaderPointer->setVec3(("spotLight[" + number + "].position").c_str(), vecProperties[0 + 6*i]);
			shaderPointer->setVec3(("spotLight[" + number + "].direction").c_str(), vecProperties[1 + 6 * i]);
			shaderPointer->setVec3(("spotLight[" + number + "].color").c_str(), vecProperties[2 + 6 * i]);
			shaderPointer->setVec3(("spotLight[" + number + "].ambient").c_str(), vecProperties[3 + 6 * i]);
			shaderPointer->setVec3(("spotLight[" + number + "].diffuse").c_str(), vecProperties[4 + 6 * i]);
			shaderPointer->setVec3(("spotLight[" + number + "].specular").c_str(), vecProperties[5 + 6 * i]);
			shaderPointer->setFloat(("spotLight[" + number + "].constant").c_str(), floatProperties[0 + 5 * i]);
			shaderPointer->setFloat(("spotLight[" + number + "].linear").c_str(), floatProperties[1 + 5 * i]);
			shaderPointer->setFloat(("spotLight[" + number + "].quadratic").c_str(), floatProperties[2 + 5 * i]);
			shaderPointer->setFloat(("spotLight[" + number + "].cutOff").c_str(), floatProperties[3 + 5 * i]);
			shaderPointer->setFloat(("spotLight[" + number + "].outerCutOff").c_str(), floatProperties[4 + 5 * i]);
		}
	}

	void setDirLightProperties(glm::vec3 vecProperties[]) {
		shaderPointer->setVec3("dirLight.direction", vecProperties[0]);
		shaderPointer->setVec3("dirLight.color", vecProperties[1]);
		shaderPointer->setVec3("dirLight.ambient", vecProperties[2]);
		shaderPointer->setVec3("dirLight.diffuse", vecProperties[3]);
		shaderPointer->setVec3("dirLight.specular", vecProperties[4]);
	}

	void setDirtLightProperties(glm::vec3 vecProperties[], unsigned int modelCount) {
		for (int i = 0; i < modelCount; i++) {
			string number = to_string(i);
			shaderPointer->setVec3(("dirLight[" + number + "].direction").c_str(), vecProperties[0 + 6 * i]);
			shaderPointer->setVec3(("dirLight[" + number + "].color").c_str(), vecProperties[1 + 6 * i]);
			shaderPointer->setVec3(("dirLight[" + number + "].ambient").c_str(), vecProperties[2 + 6 * i]);
			shaderPointer->setVec3(("dirLight[" + number + "].diffuse").c_str(), vecProperties[3 + 6 * i]);
			shaderPointer->setVec3(("dirLight[" + number + "].specular").c_str(), vecProperties[4 + 6 * i]);
		}
	}

	glm::mat4 setModelProperties(glm::vec3 translation, glm::vec3 scale, glm::vec3 rotateAxis, float degrees, glm::mat4 *model){
		if (model == NULL)
			myModel = glm::mat4(1.0f);
		else
			myModel = *model;
		myModel = glm::translate(myModel, translation);
		myModel = glm::rotate(myModel, glm::radians(degrees), rotateAxis);
		myModel = glm::scale(myModel, scale);
		shaderPointer->setMat4("model", myModel);

		return myModel;
	}

	glm::mat4 setModelProperties(glm::vec3 translation, glm::vec3 scale, glm::mat4* model) {
		if (model == NULL)
			myModel = glm::mat4(1.0f);
		else
			myModel = *model;
		myModel = glm::translate(myModel, translation);
		myModel = glm::scale(myModel, scale);
		shaderPointer->setMat4("model", myModel);

		return myModel;
	}

	void moveModel(glm::vec3 &modelPos, glm::vec3 modelScale, glm::vec3 &speedVec, float deltaTime, bool isFlying) {
		setModelProperties(modelPos, modelScale, NULL);
		if (!isFlying) {
			frictionSpeed = glm::vec3(0.0f, 0.0f, 0.0f);
			gravitySpeed *= 0.0f;
			wallBound = false;
		}

		modelSpeed = speedVec;
		if (gravityEnabled and isFlying) {
			SimulateGravity(modelPos, deltaTime);
			SimulateMove(modelPos, deltaTime);
		}
		speedVec = modelSpeed;
	}

	void SimulateGravity(glm::vec3 &modelPos, float deltaTime) {
		if (modelPos.z <= -100.0f and !wallBound) {
			modelSpeed.z *= -0.6;
			wallBound = true;
		}
		if (modelPos.y > -3.0f) {
			gravitySpeed += deltaTime * gravityForce;
			modelSpeed += gravitySpeed;
			frictionSpeed = glm::vec3(0.0f, 0.0f, 0.0f);
			floorBound = false;
		}
		else if (modelPos.y <= -3.0f) {
			SimulateFriction(modelPos, deltaTime);
			if (!floorBound) {
				modelSpeed.y = -(0.3) * (modelSpeed.y - 0.0001 * pow(modelSpeed.y, 2) / gravityForce.y);
				gravitySpeed *= 0.0f;
				floorBound = true;
			}
			if (modelSpeed.y == 0.0f)
				modelPos.y = -3.0f;
		}
	}

	void SimulateFriction(glm::vec3 &modelPos, float deltaTime) {
		frictionSpeed += deltaTime * frictionForce;
		modelSpeed += modelSpeed * frictionSpeed;
		modelSpeed *= glm::vec3(0.9f, 1.0f, 0.9f);
	}

	void SimulateMove(glm::vec3& modelPos, float deltaTime) {
		modelPos += modelSpeed * deltaTime;
	}

	~model3D() {};
};