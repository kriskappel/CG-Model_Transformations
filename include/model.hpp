#pragma once
#ifndef MODEL_HPP
#define MODEL_HPP

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include "mesh.hpp"
#include <AntTweakBar.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <glerror.hpp>

class Model {

private:	
	GLuint ModelMatrixID;
	//Texture info
	GLuint Texture;
	GLuint TextureID;

	glm::mat4 ModelMatrix;
	glm::mat4 BackUpMatrix;

	int meshIndex;

public:
	//Constructor
	Model::Model(GLuint programID, const char* textureText, const char* textureIDText, int relatedMesh, int modelIndex);
	
	GLuint getModelMatrixID();
	
	GLuint getTextureID();
	GLuint getTexture();
	void Model::setModelMatrix(glm::mat4 matrix);
	glm::mat4 Model::getModelMatrix();
	int Model::getMeshIndex();
	void Model::setMeshIndex(int index);
	void Model::printMatrix();
	void Model::setBackUpMatrix(glm::mat4 matrix);
	glm::mat4 Model::getBackUpMatrix();

	std::vector<int> flags;
};

#endif
