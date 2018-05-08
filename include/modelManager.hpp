#pragma once
#ifndef MODELMANAGER_HPP
#define MODELMANAGER_HPP

// Include standard headers

#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include AntTweakBar
#include <AntTweakBar.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <shader.hpp>
#include <texture.hpp>
#include <controls.hpp>
#include <objloader.hpp>
#include <vboindexer.hpp>
#include <glerror.hpp>

#include "mesh.hpp"
#include "model.hpp"

//Define a custom data type to represent a model
class ModelManager {

private:
	GLuint MatrixID;
	GLuint ViewMatrixID;

	int nUseMouse = 0;

	GLuint VertexArrayID;
	GLuint currentShaderProgramID;

	GLuint LightID;

	GLuint programID;

	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;
	glm::mat4 ModelMatrix;

	//double translateTime;
	//double lastTime;

	int flag = 0;

	std::vector<Model> modelVec;
	std::vector<Mesh> meshVec;
	
public:
	//Constructor
	ModelManager::ModelManager();
	void ModelManager::initializeMVP();
	GLuint getVertexArrayID();
	void setProgramID(const char * vertexShader, const char * fragmentShader);
	GLuint getProgramID();
	void ModelManager::clearScreen();
	void ModelManager::draw(Model model, Mesh mesh, unsigned int g_nWidth, unsigned int g_nHeight);
	void ModelManager::isFreeMouse(GLFWwindow* g_pWindow);
	void ModelManager::setLightID(const char * lightText);
	GLuint ModelManager::getLightID();
	GLuint getMatrixID();
	GLuint getViewMatrixID();
	void ModelManager::bufferSwap(GLFWwindow* g_pWindow);
	void ModelManager::cleanup(Model modelController);
	void ModelManager::setTranslateTime(double time);
	//void ModelManager::setLastTime(double time);
	void ModelManager::ResetMatrix();

	bool ModelManager::TranslateModel(double lastTime, glm::vec3 translacao, double translateTime, int index);
	bool ModelManager::RotateModel(double lastTime, glm::vec3 rotacao, double rotateTime, float angle, int index);
	bool ModelManager::ScaleModel(double lastTime, glm::vec3 scalar, double scaleTime, int index);
	bool ModelManager::ShearModel(double lastTime, glm::vec3 shear_vector, double shearTime, int index);
	bool ModelManager::RotationAroundPoint(double lastTime, glm::vec3 point, double time, int index);
	bool ModelManager::Proj(double lastTime, double projTime, int index);
	void ModelManager::addNewModel(GLuint programID, const char* textureText, const char* textureIDText, int relatedMesh);
	void ModelManager::addNewMesh(std::vector<glm::vec3> vertices, std::vector<glm::vec2> uvs, std::vector<glm::vec3> normals);
	Mesh ModelManager::getMesh(int index);
	Model ModelManager::getModel(int index);
	void ModelManager::bindMeshes();
	void ModelManager::atribbBuffer();
};

#endif