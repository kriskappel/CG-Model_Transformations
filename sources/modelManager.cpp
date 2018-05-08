#include "modelManager.hpp"
#include "mesh.hpp"
#include "model.hpp"
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
#include "glm/ext.hpp"
using namespace glm;

#include <shader.hpp>
#include <texture.hpp>
#include <controls.hpp>
#include <objloader.hpp>
#include <vboindexer.hpp>
#include <glerror.hpp>
#include <Windows.h>

//Constructor
ModelManager::ModelManager()
{
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

}

void ModelManager::initializeMVP() {
	MatrixID = glGetUniformLocation(programID, "MVP");
	ViewMatrixID = glGetUniformLocation(programID, "V");
}

GLuint ModelManager::getVertexArrayID() {
	return VertexArrayID;
}

GLuint ModelManager::getProgramID() {
	return programID;
}

void ModelManager::setProgramID(const char * vertexShader, const char * fragmentShader) {
	programID = LoadShaders(vertexShader, fragmentShader);
}

GLuint ModelManager::getLightID() {
	return LightID;
}

void ModelManager::setLightID(const char * lightText) {
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
}

void ModelManager::clearScreen() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/*void ModelManager::setTranslateTime(double time) {
	translateTime = time;
}

void ModelManager::setLastTime(double time) {
	lastTime = time;
}*/

void ModelManager::draw(Model model, Mesh mesh, unsigned int g_nWidth, unsigned int g_nHeight) {
	
	double time = glfwGetTime();
	
	// Compute the MVP matrix from keyboard and mouse input
	computeMatricesFromInputs(nUseMouse, g_nWidth, g_nHeight);
	for (int i = 0; i < modelVec.size(); i++)
	{
		Model modelController = modelVec[i];
		Mesh meshController = meshVec[modelController.getMeshIndex()];

		glm::mat4 ModelM = modelController.getModelMatrix();
		//std::cout << i << std::endl;
		//modelController.printMatrix();
		//std::cout << std::endl;
		/*if (i == 1) {
			//
			modelController.printMatrix();
			std::cout << std::endl;
		//	ModelM[3][0] = 0;
		}*/

		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelM;

		glUseProgram(programID);
		
		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		glUniformMatrix4fv(getMatrixID(), 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(modelController.getModelMatrixID(), 1, GL_FALSE, &ModelM[0][0]);
		glUniformMatrix4fv(getViewMatrixID(), 1, GL_FALSE, &ViewMatrix[0][0]);

		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, modelController.getTexture());
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(modelController.getTextureID(), 0);

		meshController.attributeBuffer();

		std::vector<unsigned short> * index;
		index = (meshController.getIndices());
		// Draw the triangles !
		glDrawElements(
			GL_TRIANGLES,        // mode
			(*index).size(),      // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0             // element array buffer offset
		);


		modelController.setModelMatrix(modelController.getBackUpMatrix());
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);


	// Draw tweak bars
	TwDraw();

}

void ModelManager::isFreeMouse(GLFWwindow* g_pWindow) {
	if (glfwGetKey(g_pWindow, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS)
		nUseMouse = 0;
	else
		nUseMouse = 1;
}

void ModelManager::bufferSwap(GLFWwindow* g_pWindow) {
	// Swap buffers			
	glfwSwapBuffers(g_pWindow);
	glfwPollEvents();
}

void ModelManager::cleanup(Model modelController) {
	GLuint textureReturn = modelController.getTexture();
	glDeleteProgram(programID);
	glDeleteTextures(1, &textureReturn);
	glDeleteVertexArrays(1, &VertexArrayID);
}

void ModelManager::ResetMatrix() {
	ProjectionMatrix = getProjectionMatrix();
	ViewMatrix = getViewMatrix();
	//ModelMatrix = glm::mat4(1.0);

}

bool ModelManager::TranslateModel(double lastTime, glm::vec3 translacao, double translateTime, int index) {
	double currentTime = glfwGetTime();
	bool retFlag = true;
	//bool founded = false;
	/*for (int i = 0; i < modelVec[index].flags.size(); i++)
	{
		if (modelVec[index].flags[i] == 1)
			founded = true;
	}
	if (!founded)
	{
		modelVec[index].flags.push_back(1);
	}*/

	glm::mat4 resultTrans = modelVec[index].getModelMatrix();
	glm::mat4 backup = modelVec[index].getBackUpMatrix();

	if (currentTime - lastTime <= translateTime)
	{
		//glm::vec3 translacao = glm::vec3(1, 1, 1);
		translacao.x = (translacao.x / translateTime) * (currentTime - lastTime);
		//std::cout << "x" << translacao.x;
		translacao.y = (translacao.y / translateTime) * (currentTime - lastTime);
		//std::cout << " y" << translacao.y;
		translacao.z = (translacao.z / translateTime) * (currentTime - lastTime);
		//std::cout << " z" << translacao.z<<std::endl;
		//lastTime = currentTime;
		resultTrans = glm::translate(modelVec[index].getBackUpMatrix(), translacao);
		

		retFlag = false;
	}
	if (retFlag == false)
	{
		modelVec[index].setModelMatrix(resultTrans);
	}
	else
	{
		/*for (int i = 0; i < modelVec[index].flags.size(); i++)
		{
			if (modelVec[index].flags[i] == 1)
				modelVec[index].flags.erase(modelVec[index].flags.begin() + i);
		}
		if(modelVec[index].flags.empty())*/
		modelVec[index].setBackUpMatrix(resultTrans);
	}
	
	//std::cout << glm::to_string(translacao) << std::endl;
	//Sleep(1000);
	
	return retFlag;
}
bool ModelManager::RotateModel(double lastTime, glm::vec3 rotacao, double rotateTime, float angle, int index) {
	double currentTime = glfwGetTime();
	bool retFlag = true;
	/*bool founded = false;
	for (int i = 0; i < modelVec[index].flags.size(); i++)
	{
		if (modelVec[index].flags[i] == 2)
			founded = true;
	}
	if (!founded)
	{
		modelVec[index].flags.push_back(2);
	}*/


	glm::mat4 resultRot = modelVec[index].getModelMatrix();
	glm::mat4 backup = modelVec[index].getBackUpMatrix();

	//if(modelVec[index].flags.empty())
	
	if (currentTime - lastTime <= rotateTime)
	{
		angle = (angle / rotateTime) * (currentTime - lastTime);
		
		resultRot = glm::rotate(modelVec[index].getBackUpMatrix(), angle, rotacao);
		retFlag = false;
	}
	
	if (retFlag == false)
	{
		modelVec[index].setModelMatrix(resultRot);
	}
	else
	{
		/*for (int i = 0; i < modelVec[index].flags.size(); i++)
		{
			if (modelVec[index].flags[i] == 2)
				modelVec[index].flags.erase(modelVec[index].flags.begin() + i);
		}
		if (modelVec[index].flags.empty())*/
		modelVec[index].setBackUpMatrix(resultRot);
	}
		
	return retFlag;
}

bool ModelManager::ScaleModel(double lastTime, glm::vec3 scalar, double scaleTime, int index) {
	double currentTime = glfwGetTime();
	/*if (currentTime - lastTime <= scaleTime)
	{
		//glm::vec3 translacao = glm::vec3(1, 1, 1);
		scalar.x = ((scalar.x / scaleTime) * (currentTime - lastTime))/ 2 + 0.5;
		//std::cout << "x" << translacao.x;
		scalar.y = ((scalar.y / scaleTime) * (currentTime - lastTime))/2 + 0.5;
		//std::cout << " y" << translacao.y;
		scalar.z = ((scalar.z / scaleTime) * (currentTime - lastTime))/ 2 + 0.5;
		//std::cout << " z" << translacao.z<<std::endl;
		//lastTime = currentTime;
	}*/
	
	bool retFlag = true;

	glm::mat4 resultScale = modelVec[index].getModelMatrix();

	if (currentTime - lastTime <= scaleTime)
	{
		//glm::vec3 translacao = glm::vec3(1, 1, 1);
		double raiz = ((double)(scalar.x/scaleTime * (currentTime - lastTime))) /2;
		scalar.x = pow(scalar.x, raiz); //* (currentTime - lastTime) / scaleTime);
		//std::cout << "x" << translacao.x;
		raiz = ((double)(scalar.y/scaleTime * (currentTime - lastTime))) /2;
		scalar.y =  pow(scalar.y , raiz) ;//* (currentTime - lastTime) / scaleTime);
		//std::cout << " y" << translacao.y;
		raiz = ((double)(scalar.z/scaleTime * (currentTime - lastTime))) /2;
		scalar.z = pow(scalar.z, raiz) ;//* (currentTime - lastTime) / scaleTime);
		//std::cout << " z" << translacao.z
		resultScale = glm::scale(modelVec[index].getBackUpMatrix(), scalar);
		retFlag = false;
	}
	//glm::mat4 resultScale = glm::scale(ModelMatrix, scalar);

	//std::cout << (currentTime - lastTime) << std::endl;
	//Sleep(10);
	if (retFlag == false)
	{
		modelVec[index].setModelMatrix(resultScale);
	}
	else
		modelVec[index].setBackUpMatrix(resultScale);

	return retFlag;
	//ModelMatrix = resultScale;
}

bool ModelManager::ShearModel(double lastTime, glm::vec3 shear, double shearTime, int index)
{

	bool retFlag = true;
	glm::mat4 resultShear(1.0);

	resultShear[0][1] = shear.x;
	resultShear[0][2] = shear.y;
	resultShear[0][3] = shear.z;

	glm::mat4 resultS = modelVec[index].getModelMatrix();

	double currentTime = glfwGetTime();
	if (currentTime - lastTime <= shearTime)
	{
		resultShear[0][1] = (shear.x / shearTime) * (currentTime - lastTime);
		resultShear[0][2] = (shear.y / shearTime) * (currentTime - lastTime);
		resultShear[0][3] = (shear.z / shearTime) * (currentTime - lastTime);

		resultS = resultShear * modelVec[index].getBackUpMatrix();

		retFlag = false;
	}


	if (retFlag == false)
	{
		glm::mat4 MMatrix = modelVec[index].getModelMatrix();
		modelVec[index].setModelMatrix(resultS);
	}
	else
	{
		glm::mat4 BMatrix = modelVec[index].getModelMatrix();
		modelVec[index].setBackUpMatrix(resultShear);
	}
		

	return retFlag;

}

bool ModelManager::RotationAroundPoint(double lastTime, glm::vec3 point, double time, int index)
{
	//first step, find vec to point
	glm::mat4 ModelM = modelVec[index].getModelMatrix();
	glm::vec3 currentPosition = glm::vec3(ModelM[3][0], ModelM[3][1], ModelM[3][2]);
	//std::cout << "current"<< glm::to_string(currentPosition) << std::endl;
	glm::vec3 translateVector = point - currentPosition;
	//std::cout << "next"<< glm::to_string(translateVector) << std::endl;
	//translateVector = translateVector * glm::vec3(2);
	//glm::vec3 axis = glm::cross(translateVector, vec3(0.0, 1.0, 0.0));
	glm::vec3 axis = glm::vec3(0.0, 1.0, 0.0);
	//translate to the poin
	//double lastTime = glfwGetTime();
	bool flag = TranslateModel(lastTime, translateVector, (time * (1.0/3.0)), index);
	
	bool falgR = false;

	bool flagB = false;
	/*if (axis == glm::vec3(0))
	{
		axis = glm::vec3(0, 1, 0);
	}*/
	//bool flagR=false;
	if (flag)
	{
		//std::cout << "aqui " << std::endl;

		//glm::vec3 axis = glm::vec3(0, 0, 1);
		bool flag = RotateModel(lastTime + (time * (1.0 / 3.0)), axis, (time * (1.0 / 3.0)), 360.0, index);
		if (flag)
			falgR = true;
	}

	if (falgR)
	{
		glm::vec3 newPos = glm::vec3(ModelM[3][0], ModelM[3][1], ModelM[3][2]);
		// std::cout << "aqui " << std::endl;
		glm::vec3 translateVector = currentPosition - newPos;
		bool flagB = TranslateModel(lastTime + (time * (2.0 / 3.0)), translateVector, (time * (1.0 / 3.0)), index);
	}
	//ROTATE AROUND THE POINT

	return flagB;
}

bool ModelManager::Proj(double lastTime, double projTime, int index)
{
	double currentTime = glfwGetTime();
	
	bool retFlag = true;

	glm::mat4 resultProj = modelVec[index].getModelMatrix();

	glm::vec3 proj3 = glm::vec3(0, 0, 1);
	if (currentTime - lastTime <= projTime)
	{
		proj3.z = (proj3.z / projTime) * (currentTime - lastTime);

		resultProj = glm::proj3D(modelVec[index].getBackUpMatrix(), proj3);
		retFlag = false;
	}

	if (retFlag == false)
	{
		modelVec[index].setModelMatrix(resultProj);
	}
	else
		modelVec[index].setBackUpMatrix(resultProj);

	return retFlag;
	
}

void ModelManager::addNewMesh(std::vector<glm::vec3> vertices, std::vector<glm::vec2> uvs, std::vector<glm::vec3> normals)
{
	Mesh newMesh = Mesh(vertices, uvs, normals);
	meshVec.push_back(newMesh);
}

Mesh ModelManager::getMesh(int index)
{
	return meshVec[index];
}

void ModelManager::addNewModel(GLuint programID, const char* textureText, const char* textureIDText, int relatedMesh)
{
	Model newModel = Model( programID,  textureText,  textureIDText, relatedMesh, (modelVec.size()));
	modelVec.push_back(newModel);
}

Model ModelManager::getModel(int index)
{
	return modelVec[index];
}

void ModelManager::bindMeshes()
{
	for (int i = 0; i < meshVec.size(); i++)
	{
		meshVec[i].bind();
	}
}

void ModelManager::atribbBuffer()
{
	for (int i = 0; i < meshVec.size(); i++)
	{
		meshVec[i].attributeBuffer();
	}
}

GLuint ModelManager::getViewMatrixID() {
	return ViewMatrixID;
}

GLuint ModelManager::getMatrixID() {
	return MatrixID;
}