#include "model.hpp"
#include <GL/glew.h>

#include <texture.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <AntTweakBar.h>
#include <iostream>
#include <glm/gtx/transform2.hpp>

Model::Model(GLuint programID, const char* textureText, const char* textureIDText, int relatedMesh, int modelIndex)
{
	// Get a handle for our "MVP" uniform
	ModelMatrixID = glGetUniformLocation(programID, "M");

	Texture = loadDDS(textureText);
	TextureID = glGetUniformLocation(programID, textureIDText);

	setMeshIndex(relatedMesh);

	glm::vec3 vetorTranslacao = glm::vec3(modelIndex*2, 0, 0);
	glm::mat4 initialPos = glm::translate(glm::mat4(1.0), vetorTranslacao);

	setModelMatrix(initialPos);
	setBackUpMatrix(initialPos);
}

GLuint Model::getModelMatrixID() {
	return ModelMatrixID;
}

GLuint Model::getTextureID(){
	return TextureID;
}
GLuint Model::getTexture() {
	return Texture;
}

void Model::setModelMatrix(glm::mat4 matrix)
{
	ModelMatrix = matrix;
}

glm::mat4 Model::getModelMatrix()
{
	return ModelMatrix;
}

int Model::getMeshIndex() {
	return meshIndex;
}
void Model::setMeshIndex(int index) {
	meshIndex = index;
}

void Model::printMatrix() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			std::cout << ModelMatrix[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

void Model::setBackUpMatrix(glm::mat4 matrix)
{
	BackUpMatrix = matrix;
}

glm::mat4 Model::getBackUpMatrix()
{
	return BackUpMatrix;
}