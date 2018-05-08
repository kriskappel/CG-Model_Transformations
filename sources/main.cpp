// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* g_pWindow;
unsigned int g_nWidth = 1024, g_nHeight = 768;

// Include AntTweakBar
#include <AntTweakBar.h>
TwBar *g_pToolBar;

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

#include "mesh.hpp"
#include "model.hpp"
#include "modelManager.hpp"

typedef struct tb {
	double time;
	double tVector[3];
}Translate_Bar;

typedef struct rb {
	double time;
	float angle;
	double rVector[3];
}Rotate_Bar;

typedef struct sb {
	double time;
	double sVector[3];
}Scale_Bar;

typedef struct shb {
	double time;
	double shVector[3];
}Shear_Bar;

double lastTime = glfwGetTime();

glm::vec3 translateVector = glm::vec3(0, 0, 0);
double translateTime = 0.0;
bool translateFlag = false;

glm::vec3 rotateVector = glm::vec3(0, 0, 0);
float rotateAngle = 0.0;
double rotateTime = 0.0;
bool rotateFlag = false;

glm::vec3 scaleVector = glm::vec3(0, 0, 0);
double scaleTime = 0.0;
bool scaleFlag = false;

glm::vec3 shearVector = glm::vec3(0, 0, 0);
double shearTime = 0.0;
bool shearFlag = false;

glm::vec3 pointRAP = glm::vec3(0, 0, 0);
double RAPTime = 0.0;
bool RAPflag = false;

double projTime = 0.0;
bool projFlag = false;

int modelShape = 0.0;
bool modelFlag = false;

bool generalFlag = false;

/*ModelManager managerController;
Model modelController;
Mesh meshController;*/

void WindowSizeCallBack(GLFWwindow *pWindow, int nWidth, int nHeight) {

	g_nWidth = nWidth;
	g_nHeight = nHeight;
	glViewport(0, 0, g_nWidth, g_nHeight);
	TwWindowSize(g_nWidth, g_nHeight);
}

void OkTranslate(void * tData)
{
	Translate_Bar * translateData = (Translate_Bar * ) tData;
	double time = (translateData->time);
	double x1 = translateData->tVector[0];
	double x2 = translateData->tVector[1];
	double x3 = translateData->tVector[2];
	translateTime = time;
	translateVector = glm::vec3(x1, x2, x3);
	translateFlag = true;
	lastTime = glfwGetTime();
	std::cout <<time << " " << x1 << " " << x2 << " " << x3 << std::endl;

}

void OkRotate(void * rData)
{
	Rotate_Bar * rotateData = (Rotate_Bar *)rData;
	rotateTime = rotateData->time;
	rotateVector = glm::vec3(rotateData->rVector[0], rotateData->rVector[1], rotateData->rVector[2]);
	rotateFlag = true;
	rotateAngle = rotateData->angle;

	lastTime = glfwGetTime();
}

void OkScale(void * sData)
{
	Scale_Bar * scaleData = (Scale_Bar *) sData;
	scaleTime = scaleData->time;
	scaleVector = glm::vec3(scaleData->sVector[0], scaleData->sVector[1], scaleData->sVector[2]);
	scaleFlag = true;

	lastTime = glfwGetTime();
}

void OkShear(void * shData)
{
	Shear_Bar * shearData = (Shear_Bar *)shData;
	shearTime = shearData->time;
	shearVector = glm::vec3(shearData->shVector[0], shearData->shVector[1], shearData->shVector[2]);
	shearFlag = true;

	lastTime = glfwGetTime();
}

void OkRAP(void * data)
{
	Translate_Bar * RAPData = (Translate_Bar *)data;

	RAPTime = RAPData->time;
	pointRAP = glm::vec3(RAPData->tVector[0], RAPData->tVector[1], RAPData->tVector[2]);
	RAPflag = true;
	lastTime = glfwGetTime();
}

void OkProj(void * data)
{
	double * pTime = (double * )data;
	projTime = *pTime;

	std::cout << projTime;
	projFlag = true;
	lastTime = glfwGetTime();
}

void AddModel(void * data)
{
	int * codeModel = (int*)data;

	modelShape = * codeModel;
	modelFlag = true;
}

void go(void *)
{
	generalFlag = true;
	lastTime = glfwGetTime();
}

int main(void)
{

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	g_pWindow = glfwCreateWindow(g_nWidth, g_nHeight, "CG UFPel", NULL, NULL);
	if (g_pWindow == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(g_pWindow);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	check_gl_error();//OpenGL error from GLEW

	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(g_nWidth, g_nHeight);

	// Set GLFW event callbacks. I removed glfwSetWindowSizeCallback for conciseness
	glfwSetMouseButtonCallback(g_pWindow, (GLFWmousebuttonfun)TwEventMouseButtonGLFW); // - Directly redirect GLFW mouse button events to AntTweakBar
	glfwSetCursorPosCallback(g_pWindow, (GLFWcursorposfun)TwEventMousePosGLFW);          // - Directly redirect GLFW mouse position events to AntTweakBar
	glfwSetScrollCallback(g_pWindow, (GLFWscrollfun)TwEventMouseWheelGLFW);    // - Directly redirect GLFW mouse wheel events to AntTweakBar
	glfwSetKeyCallback(g_pWindow, (GLFWkeyfun)TwEventKeyGLFW);                         // - Directly redirect GLFW key events to AntTweakBar
	glfwSetCharCallback(g_pWindow, (GLFWcharfun)TwEventCharGLFW);                      // - Directly redirect GLFW char events to AntTweakBar
	glfwSetWindowSizeCallback(g_pWindow, WindowSizeCallBack);
	
	//create the toolbar
	g_pToolBar = TwNewBar("CG UFPel ToolBar");
	
	// Add 'speed' to 'bar': it is a modifable (RW) variable of type TW_TYPE_DOUBLE. Its key shortcuts are [s] and [S].
	double speed = 0.0;
	// double translateTime = 5.0;
	Translate_Bar *translateInfo = (Translate_Bar*)malloc(sizeof(Translate_Bar));
	Rotate_Bar *rotateInfo = (Rotate_Bar*)malloc(sizeof(Rotate_Bar));
	Scale_Bar * scaleInfo = (Scale_Bar *)malloc(sizeof(Scale_Bar));
	Shear_Bar * shearInfo = (Shear_Bar *)malloc(sizeof(Shear_Bar));
	double * projectTime = (double *)malloc(sizeof(double));
	int * modelToAdd = (int *)malloc(sizeof(int));

	translateInfo->time = 0;
	rotateInfo->time = 0;
	rotateInfo->angle = 0;
	scaleInfo->time = 0;
	shearInfo->time = 0;
	*projectTime = 0;
	*modelToAdd = 0;
	for (int i = 0; i < 3; i++)
	{
		translateInfo->tVector[i] = 0;
		rotateInfo->rVector[i] = 0;
		scaleInfo->sVector[i] = 0;
		shearInfo->shVector[i] = 0;
	}
	/*
	translateInfo->tVector.x = 0;
	translateInfo->tVector.y = 0;
	translateInfo->tVector.z = 0;*/

	//double tTime = 0.0;
	//glm::vec3 translateVector;
	TwAddVarRW(g_pToolBar, "speed", TW_TYPE_DOUBLE, &speed, " label='Rot speed' min=0 max=2 step=0.01 keyIncr=s keyDecr=S help='Rotation speed (turns/second)' ");
	// Add 'bgColor' to 'bar': it is a modifable variable of type TW_TYPE_COLOR3F (3 floats color)
	vec3 oColor(0.0f);
	TwAddVarRW(g_pToolBar, "bgColor", TW_TYPE_COLOR3F, &oColor[0], " label='Background color' ");
		
	TwAddSeparator(g_pToolBar, "ADD MODEL", NULL);

	TwAddVarRW(g_pToolBar, "Model", TW_TYPE_INT8, modelToAdd, " min=0.0 max=2.0 step=1.0 help='0= SUZANNE; 1= CUBE; 2=GOOSE' ");
	TwAddButton(g_pToolBar, "Choose", AddModel, (void*)modelToAdd, "");

	TwAddSeparator(g_pToolBar, "Go", NULL);
	TwAddButton(g_pToolBar, "GOOOO", go, NULL, "");

	TwAddSeparator(g_pToolBar, "Translate", NULL);
	
	TwAddVarRW(g_pToolBar, "Translate Time", TW_TYPE_DOUBLE, &translateInfo->time , " label='Time' min=0.0 max=10.0 step=0.1 keyIncr=t keyDecr=T help='seconds' ");
	TwAddVarRW(g_pToolBar, "Translate x", TW_TYPE_DOUBLE, &translateInfo->tVector[0], " label='x' min=-15.0 max=15.0 step=1.0 keyIncr=x keyDecr=X help='translate x' ");
	TwAddVarRW(g_pToolBar, "Translate y", TW_TYPE_DOUBLE, &translateInfo->tVector[1], " label='y' min=0.0 max=15.0 step=1.0 keyIncr=y keyDecr=Y help='translate y' ");
	TwAddVarRW(g_pToolBar, "Translate z", TW_TYPE_DOUBLE, &translateInfo->tVector[2], " label='z' min=0.0 max=15.0 step=1.0 keyIncr=z keyDecr=Z help='translate z' ");
	
	TwAddButton(g_pToolBar, "translacao", OkTranslate, (void *) translateInfo, "");

	TwAddSeparator(g_pToolBar, "Rotation around point", NULL);
	TwAddButton(g_pToolBar, "rotate around point", OkRAP, (void*)translateInfo, "");

	TwAddSeparator(g_pToolBar, "Rotate", NULL);

	TwAddVarRW(g_pToolBar, "Rotate Time", TW_TYPE_DOUBLE, &rotateInfo->time, " label='Time' min=0.0 max=10.0 step=0.1 keyIncr=t keyDecr=T help='seconds' ");
	TwAddVarRW(g_pToolBar, "Rotate Angle", TW_TYPE_FLOAT, &rotateInfo->angle, " label='Angle' min=0.0 max=360.0 step=15.0 keyIncr=a keyDecr=A help='angle' ");
	TwAddVarRW(g_pToolBar, "Rotate x", TW_TYPE_DOUBLE, &rotateInfo->rVector[0], " label='x' min=0.0 max=1.0 step=1.0 keyIncr=x keyDecr=X help='translate x' ");
	TwAddVarRW(g_pToolBar, "Rotate y", TW_TYPE_DOUBLE, &rotateInfo->rVector[1], " label='y' min=0.0 max=1.0 step=1.0 keyIncr=y keyDecr=Y help='translate y' ");
	TwAddVarRW(g_pToolBar, "Rotate z", TW_TYPE_DOUBLE, &rotateInfo->rVector[2], " label='z' min=0.0 max=1.0 step=1.0 keyIncr=z keyDecr=Z help='translate z' ");

	TwAddButton(g_pToolBar, "rotacao", OkRotate, (void *)rotateInfo, "");

	TwAddSeparator(g_pToolBar, "Scale", NULL);

	TwAddVarRW(g_pToolBar, "Scale Time", TW_TYPE_DOUBLE, &scaleInfo->time, " label='Time' min=0.0 max=10.0 step=0.1 keyIncr=t keyDecr=T help='seconds' ");
	TwAddVarRW(g_pToolBar, "Scale x", TW_TYPE_DOUBLE, &scaleInfo->sVector[0], " label='x' min=-15.0 max=15.0 step=1.0 keyIncr=x keyDecr=X help='translate x' ");
	TwAddVarRW(g_pToolBar, "Scale y", TW_TYPE_DOUBLE, &scaleInfo->sVector[1], " label='y' min=-15.0 max=15.0 step=1.0 keyIncr=y keyDecr=Y help='translate y' ");
	TwAddVarRW(g_pToolBar, "Scale z", TW_TYPE_DOUBLE, &scaleInfo->sVector[2], " label='z' min=-15.0 max=15.0 step=1.0 keyIncr=z keyDecr=Z help='translate z' ");

	TwAddButton(g_pToolBar, "scale", OkScale, (void *)scaleInfo, "");

	TwAddSeparator(g_pToolBar, "Shear", NULL);

	TwAddVarRW(g_pToolBar, "Shear Time", TW_TYPE_DOUBLE, &shearInfo->time, " label='Time' min=0.0 max=10.0 step=0.1 keyIncr=t keyDecr=T help='seconds' ");
	TwAddVarRW(g_pToolBar, "Shear x", TW_TYPE_DOUBLE, &shearInfo->shVector[0], " label='x' min=-15.0 max=15.0 step=1.0 keyIncr=x keyDecr=X help='translate x' ");
	TwAddVarRW(g_pToolBar, "Shear y", TW_TYPE_DOUBLE, &shearInfo->shVector[1], " label='y' min=-15.0 max=15.0 step=1.0 keyIncr=y keyDecr=Y help='translate y' ");
	TwAddVarRW(g_pToolBar, "Shear z", TW_TYPE_DOUBLE, &shearInfo->shVector[2], " label='z' min=-15.0 max=15.0 step=1.0 keyIncr=z keyDecr=Z help='translate z' ");

	TwAddButton(g_pToolBar, "shear", OkShear, (void *)shearInfo, "");

	TwAddSeparator(g_pToolBar, "Project", NULL);
	TwAddVarRW(g_pToolBar, "Project Time", TW_TYPE_DOUBLE, projectTime, " label='Time' min=0.0 max=10.0 step=0.1 keyIncr=t keyDecr=T help='seconds' ");
	TwAddButton(g_pToolBar, "proj", OkProj, (void *)projectTime, "");
	
	//std::cout << translateInfo->time << glm::to_string(translateInfo->tVector) << std::endl;

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(g_pWindow, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(g_pWindow, g_nWidth / 2, g_nHeight / 2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	ModelManager managerController = ModelManager();

	char *vertexShader = "shaders/StandardShading.vertexshader";
	char *fragmentShader = "shaders/StandardShading.fragmentshader";
	managerController.setProgramID(vertexShader, fragmentShader);

	// Load the texture
	char *textureText = "mesh/uvmap.DDS";

	// Get a handle for our "myTextureSampler" uniform
	char *textureIDText = "myTextureSampler";
	
	
	//Model modelController2 = Model(managerController.getProgramID(), textureText, textureIDText);

	// Read our .obj file
	std::vector<glm::vec3> verticesSuzanne;
	std::vector<glm::vec2> uvsSuzanne;
	std::vector<glm::vec3> normalsSuzanne;
	bool res1 = loadOBJ("mesh/suzanne.obj", verticesSuzanne, uvsSuzanne, normalsSuzanne);

	std::vector<glm::vec3> verticesCube;
	std::vector<glm::vec2> uvsCube;
	std::vector<glm::vec3> normalsCube;
	bool res2 = loadOBJ("mesh/cube.obj", verticesCube, uvsCube, normalsCube);

	std::vector<glm::vec3> verticesGoose;
	std::vector<glm::vec2> uvsGoose;
	std::vector<glm::vec3> normalsGoose;
	bool res3 = loadOBJ("mesh/goose.obj", verticesGoose, uvsGoose, normalsGoose);

	//Mesh meshController = Mesh(verticesSuzanne, uvsSuzanne, normalsSuzanne);
	
	managerController.addNewMesh(verticesSuzanne, uvsSuzanne, normalsSuzanne);
	managerController.addNewMesh(verticesCube, uvsCube, normalsCube);
	managerController.addNewMesh(verticesGoose, uvsGoose, normalsGoose);

	Mesh meshController = managerController.getMesh(0);

	managerController.addNewModel(managerController.getProgramID(), textureText, textureIDText, 0);
	managerController.addNewModel(managerController.getProgramID(), textureText, textureIDText, 1);
	
	managerController.initializeMVP();

	Model modelController = managerController.getModel(0);

	//Model modelController = Model(managerController.getProgramID(), textureText, textureIDText, 0);

	managerController.bindMeshes();
	
	// Get a handle for our "LightPosition" uniform
	glUseProgram(managerController.getProgramID());
	char * lightText = "LightPosition_worldspace";
	managerController.setLightID(lightText);
	//GLuint LightID = glGetUniformLocation(managerController.getProgramID(), );

	// For speed computation
	//lastTime = glfwGetTime();
	int    nbFrames		 = 0;
	//double lastTime2	 = glfwGetTime();
	//translateTime = 5.0;
	//managerController.setTranslateTime(translateTime);
	//managerController.setLastTime(glfwGetTime());

	do{
        check_gl_error();

        //use the control key to free the mouse
		managerController.isFreeMouse(g_pWindow);

		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		//if (currentTime - lastTime >= 1.0){ // If last prinf() was more than 1sec ago
			// printf and reset
			//printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			//nbFrames  = 0;
			//lastTime += 1.0;
		//}

		// Clear the screen
		//===TODO====
		//FAZER FUNCAO RESET MODELMATRIX
		//TESTAR A FLAG E FAZER FUNCAO Q MULTIPLICA CADA MATRIZ
		//DEPOIS CHAMAR O DRAW

		managerController.clearScreen();
		// Use our shader

		managerController.ResetMatrix();
		if (modelFlag)
		{
			managerController.addNewModel(managerController.getProgramID(), textureText, textureIDText, modelShape);
			modelFlag = 0;
		}


		if (generalFlag)
		{
			
			if (translateFlag)
			{
				bool ret = managerController.TranslateModel(lastTime, translateVector, translateTime, *modelToAdd);
				if (ret)
				{
					translateFlag = false;
				}
			}

			if (rotateFlag)
			{
				bool ret = managerController.RotateModel(lastTime, rotateVector, rotateTime, rotateAngle, *modelToAdd);
				if (ret)
				{
					rotateFlag = false;
				}
			}

			if (scaleFlag)
			{
				bool ret = managerController.ScaleModel(lastTime, scaleVector, scaleTime, *modelToAdd);
				if (ret)
				{
					scaleFlag = false;
				}
			}

			if (shearFlag)
			{
				bool ret = managerController.ShearModel(lastTime, shearVector, shearTime, *modelToAdd);
				if (ret)
				{
					shearFlag = false;
				}
			}

			if (RAPflag)
			{
				bool ret = managerController.RotationAroundPoint(lastTime, pointRAP, RAPTime, *modelToAdd);
				if (ret)
				{
					RAPflag = false;
				}
			}

			if (projFlag)
			{
				bool ret = managerController.Proj(lastTime, projTime, *modelToAdd);
				if (ret)
				{
					projFlag = false;
				}
			}

			if (!projFlag && !RAPflag && !shearFlag && !scaleFlag && !rotateFlag && !translateFlag)
				generalFlag = false;
		}
		managerController.draw(modelController, meshController, g_nWidth, g_nHeight);
	
		//meshController.attributeBuffer();

		managerController.bufferSwap(g_pWindow);

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(g_pWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(g_pWindow) == 0);

	// Cleanup VBO and shader
	meshController.cleanup();

	managerController.cleanup(modelController);

	// Terminate AntTweakBar and GLFW
	TwTerminate();
	glfwTerminate();

	return 0;
}

