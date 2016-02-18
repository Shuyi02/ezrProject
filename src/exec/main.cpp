// Include standard headers
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Eigen/Dense>
#include <Eigen/Core>

#include <iostream>
using namespace std;

#define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/constants.hpp"

#include "utils/Control.h"
#include "utils/Model.h"
#include "utils/ShaderLoad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "utils/stb_image.h"

#include "utils/TextureManager.h"

//not nice but camera mouse controls work HACK
GLFWwindow* window;

int windowHeight = 768;
int windowWidth = 1024;

void calcSphereCoords(glm::vec3& coords, float theta, float phi, float radius){
	coords.x = radius * glm::cos(theta) * glm::sin(phi);
	coords.y = radius * glm::cos(phi);
	coords.z = radius * glm::sin(theta) * glm::sin(phi);
}

int main() {

	// --------------------------------------------------------------------------------- setup
	//initialize GLFW
	if (!glfwInit()) {
		fprintf( stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //we don't want the old OpenGL

	//open window and create its OpenGL context
	window = glfwCreateWindow(windowWidth, windowHeight, "myEZR", NULL, NULL);

	if (window == NULL) {
		fprintf( stderr,
				"Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}

	// initialize GLEW
	glfwMakeContextCurrent(window);
	glewExperimental = true; // needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	//ensure we capture escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	//hide the mouse and enable unlimited movement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	//depth test
	glEnable(GL_DEPTH_TEST);
	//accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	//cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// --------------------------------------------------------------------- Load Model and Texture

//	utils::Model *ml_ogre = new utils::Model(RESOURCES_PATH "/Models/imrod/ImrodLowPoly.obj");
//	utils::Model *ml_sphere = new utils::Model(RESOURCES_PATH "/Models/sphere/sphere.obj");
//	utils::Model *ml_suzanne = new utils::Model(RESOURCES_PATH "/Models/suzanne/suzanne.obj");
//	utils::Model *ml_tetris= new utils::Model(RESOURCES_PATH "/Models/tetris/tetris.obj");
//	utils::Model *ml_torus= new utils::Model(RESOURCES_PATH "/Models/torus/torus.obj");
	utils::Model *ml_lightSphere = new utils::Model(RESOURCES_PATH "/Models/sphere/lightOrb.obj",1);
	utils::Model *ml_teapot = new utils::Model(RESOURCES_PATH "/Models/teapot/pot2.obj",1);
	utils::Model *ml_teapotGlobalDir = new utils::Model(RESOURCES_PATH "/Models/teapot/pot2.obj",2);
	utils::Model *currentModel = ml_teapot;

	GLuint texture_kitty = utils::loadTexture(RESOURCES_PATH"/kitty.jpg");

	std::string texturePath = RESOURCES_PATH"/hatchesLess02/hatch";
//	std::string texturePath = RESOURCES_PATH"/dots01/dots";
	int mipMapLevel = 4;
	GLuint texture_hatch00 = utils::loadMipMapTexture(texturePath+"0", mipMapLevel);
	GLuint texture_hatch01 = utils::loadMipMapTexture(texturePath+"1", mipMapLevel);
	GLuint texture_hatch02 = utils::loadMipMapTexture(texturePath+"2", mipMapLevel);
	GLuint texture_hatch03 = utils::loadMipMapTexture(texturePath+"3", mipMapLevel);
	GLuint texture_hatch04 = utils::loadMipMapTexture(texturePath+"4", mipMapLevel);
	GLuint texture_hatch05 = utils::loadMipMapTexture(texturePath+"5", mipMapLevel);


	//============================================================================ shaders and setups for shadowMap
	GLuint depthProgramID = utils::loadShaders( SHADERS_PATH "/depthShadow.vert", SHADERS_PATH "/depthShadow.frag" );

	//mvp uniform
	GLuint depthMatrixID = glGetUniformLocation(depthProgramID, "depthMVP");

	GLuint FramebufferShadow = 1;
	glGenFramebuffers(1, &FramebufferShadow);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferShadow);

	//depth texture
	GLuint depthTexture;
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, windowWidth, windowWidth, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	glDrawBuffer(GL_NONE); // No color buffer is drawn to.

	//always check if framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	return false;


	//====================================================================================== shaders and setup for outline
	GLuint outlineProgramID = utils::loadShaders( SHADERS_PATH "/preOutline.vert", SHADERS_PATH "/preOutline.frag" );
	glUseProgram(outlineProgramID);

	//mvp uniform
	GLuint outlineMatrixID = glGetUniformLocation(outlineProgramID, "normalDepthMVP");
	GLuint outlineMVinvertiertMatrixID = glGetUniformLocation(outlineProgramID, "mv_ti");

	//framebuffer for outline
	GLuint FramebufferOutline = 2;
	glGenFramebuffers(1, &FramebufferOutline);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferOutline);

	//texture for depth and normals from camera view
	GLuint normalDepthTexture;
	glGenTextures(1, &normalDepthTexture);
	glBindTexture(GL_TEXTURE_2D, normalDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowWidth, windowWidth, 0, GL_RGBA, GL_FLOAT, 0); //RGBA 4 Parameter, Aufloesung 1024x1024 als Angabe fuer Texture2D
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); //wrapping: repeat,clamp,.. in u Richtung
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); //wrapping: repeat,clamp,.. in v Richtung
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //nimmt naechsten Pixelwert und interpoliert nich von den 4 nachbarn
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // da wir Aufoesung entsprechend Tex angelegt haben sollte das passen

	 //depth buffer
	 GLuint depthrenderbuffer;
	 glGenRenderbuffers(1, &depthrenderbuffer);
	 glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	 glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowWidth);
	 glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	 //set texture as colour attachement #0
	 glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, normalDepthTexture, 0);

	 //set the list of draw buffers
	 GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	 glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	// always check framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	return false;

	//============================================================================== shaders and setup for main hatching
	GLuint programID = utils::loadShaders( SHADERS_PATH "/hatch.vert",
		SHADERS_PATH "/hatch.frag");

	//---------------------------------------------------------------- lightPosition
	float phi = glm::pi<float>()/4;
	float theta = 0.0;
	float radius = 10.0;
	glm::vec3 lightPos = glm::vec3(-8.0f, 6.0f, 0.0f);
	calcSphereCoords(lightPos, theta, phi, radius);
	GLuint lightID = glGetUniformLocation(programID, "lightPos_Model");

	// --------------------------------------------------------------- Uniforms
	//mvp etc uniform
	GLuint MatrixM = glGetUniformLocation(programID, "m");
	GLuint MatrixV = glGetUniformLocation(programID, "v");
	GLuint MatrixP = glGetUniformLocation(programID, "p");
	GLuint MatrixMV_ti = glGetUniformLocation(programID, "mv_ti");
	GLuint depthMVPID = glGetUniformLocation(programID, "depthMVP");

	//texture sampler uniform
	GLuint hatch00ID = glGetUniformLocation(programID, "hatch00");
	GLuint hatch01ID = glGetUniformLocation(programID, "hatch01");
	GLuint hatch02ID = glGetUniformLocation(programID, "hatch02");
	GLuint hatch03ID = glGetUniformLocation(programID, "hatch03");
	GLuint hatch04ID = glGetUniformLocation(programID, "hatch04");
	GLuint hatch05ID = glGetUniformLocation(programID, "hatch05");
	GLuint shadowMapID = glGetUniformLocation(programID, "shadowMap");
	GLuint normalDepthID = glGetUniformLocation(programID, "normalDepth");//outline

	//-================================================================================ shaders for simple lightbulb
	GLuint lightProgramID = utils::loadShaders( SHADERS_PATH "/minimal.vert",
	SHADERS_PATH "/minimal.frag");

	//mvp uniform
	GLuint lightMatrixM = glGetUniformLocation(lightProgramID, "m");
	GLuint lightMatrixV = glGetUniformLocation(lightProgramID, "v");
	GLuint lightMatrixP = glGetUniformLocation(lightProgramID, "p");

	//===========================
	//1 only blending
	//============================================================================== shaders and setup for only hatching
	GLuint programID1 = utils::loadShaders( SHADERS_PATH "/hatch.vert",
		SHADERS_PATH "/hatch1Only.frag");

	//matrix uniforms
	GLuint MatrixM1 = glGetUniformLocation(programID1, "m");
	GLuint MatrixV1 = glGetUniformLocation(programID1, "v");
	GLuint MatrixP1 = glGetUniformLocation(programID1, "p");
	GLuint MatrixMV_ti1 = glGetUniformLocation(programID1, "mv_ti");
	GLuint depthMVPID1 = glGetUniformLocation(programID1, "depthMVP");
	GLuint lightID1 = glGetUniformLocation(programID1, "lightPos_Model");

	//texture sampler uniform
	GLuint hatch00ID1 = glGetUniformLocation(programID1, "hatch00");
	GLuint hatch01ID1 = glGetUniformLocation(programID1, "hatch01");
	GLuint hatch02ID1 = glGetUniformLocation(programID1, "hatch02");
	GLuint hatch03ID1 = glGetUniformLocation(programID1, "hatch03");
	GLuint hatch04ID1 = glGetUniformLocation(programID1, "hatch04");
	GLuint hatch05ID1 = glGetUniformLocation(programID1, "hatch05");

	//===========================
	//2 threshold
	//============================================================================== shaders only hatching+thresh
	GLuint programID2 = utils::loadShaders( SHADERS_PATH "/hatch.vert",
		SHADERS_PATH "/hatch2Threshold.frag");

	//matrix uniforms
	GLuint MatrixM2 = glGetUniformLocation(programID2, "m");
	GLuint MatrixV2 = glGetUniformLocation(programID2, "v");
	GLuint MatrixP2 = glGetUniformLocation(programID2, "p");
	GLuint MatrixMV_ti2 = glGetUniformLocation(programID2, "mv_ti");
	GLuint depthMVPID2 = glGetUniformLocation(programID2, "depthMVP");
	GLuint lightID2 = glGetUniformLocation(programID2, "lightPos_Model");

	//texture sampler uniform
	GLuint hatch00ID2 = glGetUniformLocation(programID2, "hatch00");
	GLuint hatch01ID2 = glGetUniformLocation(programID2, "hatch01");
	GLuint hatch02ID2 = glGetUniformLocation(programID2, "hatch02");
	GLuint hatch03ID2 = glGetUniformLocation(programID2, "hatch03");
	GLuint hatch04ID2 = glGetUniformLocation(programID2, "hatch04");
	GLuint hatch05ID2 = glGetUniformLocation(programID2, "hatch05");

	//===========================
	//3 outline
	//============================================================================== shaders for hatching + outline
	GLuint programID3 = utils::loadShaders( SHADERS_PATH "/hatch.vert",
		SHADERS_PATH "/hatch3Outline.frag");

	//matrix uniforms
	GLuint MatrixM3 = glGetUniformLocation(programID3, "m");
	GLuint MatrixV3 = glGetUniformLocation(programID3, "v");
	GLuint MatrixP3 = glGetUniformLocation(programID3, "p");
	GLuint MatrixMV_ti3 = glGetUniformLocation(programID3, "mv_ti");
	GLuint depthMVPID3 = glGetUniformLocation(programID3, "depthMVP");
	GLuint lightID3 = glGetUniformLocation(programID3, "lightPos_Model");

	//texture sampler uniform
	GLuint hatch00ID3 = glGetUniformLocation(programID3, "hatch00");
	GLuint hatch01ID3 = glGetUniformLocation(programID3, "hatch01");
	GLuint hatch02ID3 = glGetUniformLocation(programID3, "hatch02");
	GLuint hatch03ID3 = glGetUniformLocation(programID3, "hatch03");
	GLuint hatch04ID3 = glGetUniformLocation(programID3, "hatch04");
	GLuint hatch05ID3 = glGetUniformLocation(programID3, "hatch05");
	GLuint shadowMapID3 = glGetUniformLocation(programID3, "shadowMap");
	GLuint normalDepthID3 = glGetUniformLocation(programID3, "normalDepth");//outline

	//===========================
	//4 shadow
	//============================================================================== shaders for hatching + shadow
	GLuint programID4 = utils::loadShaders( SHADERS_PATH "/hatch.vert",
		SHADERS_PATH "/hatch4Shadow.frag");

	//matrix uniforms
	GLuint MatrixM4 = glGetUniformLocation(programID4, "m");
	GLuint MatrixV4 = glGetUniformLocation(programID4, "v");
	GLuint MatrixP4 = glGetUniformLocation(programID4, "p");
	GLuint MatrixMV_ti4 = glGetUniformLocation(programID4, "mv_ti");
	GLuint depthMVPID4 = glGetUniformLocation(programID4, "depthMVP");
	GLuint lightID4 = glGetUniformLocation(programID4, "lightPos_Model");

	//texture sampler uniform
	GLuint hatch00ID4 = glGetUniformLocation(programID4, "hatch00");
	GLuint hatch01ID4 = glGetUniformLocation(programID4, "hatch01");
	GLuint hatch02ID4 = glGetUniformLocation(programID4, "hatch02");
	GLuint hatch03ID4 = glGetUniformLocation(programID4, "hatch03");
	GLuint hatch04ID4 = glGetUniformLocation(programID4, "hatch04");
	GLuint hatch05ID4 = glGetUniformLocation(programID4, "hatch05");
	GLuint shadowMapID4 = glGetUniformLocation(programID4, "shadowMap");
	GLuint normalDepthID4 = glGetUniformLocation(programID4, "normalDepth");//outline

	//===========================
	//9
	//============================================================================== shaders everything except threshold
	GLuint programID9 = utils::loadShaders( SHADERS_PATH "/hatch.vert",
		SHADERS_PATH "/hatch9NoThresh.frag");

	//matrix uniforms
	GLuint MatrixM9 = glGetUniformLocation(programID9, "m");
	GLuint MatrixV9 = glGetUniformLocation(programID9, "v");
	GLuint MatrixP9 = glGetUniformLocation(programID9, "p");
	GLuint MatrixMV_ti9 = glGetUniformLocation(programID9, "mv_ti");
	GLuint depthMVPID9 = glGetUniformLocation(programID9, "depthMVP");
	GLuint lightID9 = glGetUniformLocation(programID9, "lightPos_Model");

	//texture sampler uniform
	GLuint hatch00ID9 = glGetUniformLocation(programID9, "hatch00");
	GLuint hatch01ID9 = glGetUniformLocation(programID9, "hatch01");
	GLuint hatch02ID9 = glGetUniformLocation(programID9, "hatch02");
	GLuint hatch03ID9 = glGetUniformLocation(programID9, "hatch03");
	GLuint hatch04ID9 = glGetUniformLocation(programID9, "hatch04");
	GLuint hatch05ID9 = glGetUniformLocation(programID9, "hatch05");
	GLuint shadowMapID9 = glGetUniformLocation(programID9, "shadowMap");
	GLuint normalDepthID9 = glGetUniformLocation(programID9, "normalDepth");//outline

	//============================================================================================= rendering loop

	int sceneType = 0;

	do {
		//model with curvature
		if(glfwGetKey(window, GLFW_KEY_M ) == GLFW_PRESS){
			currentModel = ml_teapot;
		}
		//model without curvature
		if(glfwGetKey(window, GLFW_KEY_N ) == GLFW_PRESS){
			currentModel = ml_teapotGlobalDir;
		}
		//standard shader with everything!
		if(glfwGetKey(window, GLFW_KEY_0 ) == GLFW_PRESS){
			sceneType = 0;
		}
		//shader with nothing but hatches
		if(glfwGetKey(window, GLFW_KEY_1 ) == GLFW_PRESS){
			sceneType = 1;
		}
		//shader with hatches + thresh
		if(glfwGetKey(window, GLFW_KEY_2 ) == GLFW_PRESS){
			sceneType = 2;
		}
		//shader with hatches + thresh + thresh + outline
		if(glfwGetKey(window, GLFW_KEY_3 ) == GLFW_PRESS){
			sceneType = 3;
		}
		//shader with hatches + thresh + outline + shadow
		if(glfwGetKey(window, GLFW_KEY_4 ) == GLFW_PRESS){
			sceneType = 4;
		}
		//shader with everything except threshold
		if(glfwGetKey(window, GLFW_KEY_9 ) == GLFW_PRESS){
			sceneType = 9;
		}

		// --------------------------------------------------------------------------  render to texture shadowmap
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferShadow);
		glViewport(0,0,windowWidth,windowWidth);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// use shader for shadowMap
		glUseProgram(depthProgramID);

		glm::mat4 depthProjectionMatrix = glm::perspective(glm::radians(100.0f), 1.0f, 0.1f, 100.0f);
//		glm::mat4 depthProjectionMatrix = glm::ortho(-10.f,10.f,-10.f,10.f,0.0001f,100.f);
		glm::mat4 depthViewMatrix = glm::lookAt(lightPos, glm::vec3(0,0,0), glm::vec3(0,1,0));
		glm::mat4 depthModelMatrix = glm::mat4(1.0);
		glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

		// send transformation to the currently bound shader
		glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);

		currentModel->render();
		// --------------------------------
		//matrices
				utils::computeMatricesFromInputs();
				glm::mat4 m = glm::mat4(1.0);
				glm::mat4 v = utils::getViewMatrix();
				glm::mat4 p = utils::getProjectionMatrix();
				glm::mat4 mv_ti = glm::transpose(glm::inverse(v*m));
				glm::mat4 biasMatrix(
							0.5, 0.0, 0.0, 0.0,
							0.0, 0.5, 0.0, 0.0,
							0.0, 0.0, 0.5, 0.0,
							0.5, 0.5, 0.5, 1.0);
				glm::mat4 depthBiasMVP = biasMatrix*depthMVP;

		//------------------------------------------------------------------------------ Render to texture outline stuff
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferOutline);
		glViewport(0,0,windowWidth,windowWidth);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(outlineProgramID);

		glm::mat4 normalDepthMVP = p*v*m;

		glUniformMatrix4fv(outlineMatrixID, 1, GL_FALSE, &normalDepthMVP[0][0]);
		glUniformMatrix4fv(outlineMVinvertiertMatrixID, 1, GL_FALSE, &mv_ti[0][0]);

		//draw model
		currentModel->render();

		//---------------------------------------------------------------------------- render to the screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0,0,windowWidth, windowHeight);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		//clear the screen
		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		switch (sceneType) {
			case 0:
				//use main shader
				glUseProgram(programID);

				//send transformations to currently bound shader in the mvp uniform
				glUniformMatrix4fv(MatrixM, 1, GL_FALSE, &m[0][0]);
				glUniformMatrix4fv(MatrixV, 1, GL_FALSE, &v[0][0]);
				glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &p[0][0]);
				glUniformMatrix4fv(MatrixMV_ti, 1, GL_FALSE, &mv_ti[0][0]);
				glUniformMatrix4fv(depthMVPID, 1, GL_FALSE, &depthMVP[0][0]);

				//-------------------------------------------------------------------------------- Textures
				glBindTexture(GL_TEXTURE_2D, texture_hatch00);
				glUniform1i(hatch00ID, 0);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, texture_hatch01);
				glUniform1i(hatch01ID, 1);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, texture_hatch02);
				glUniform1i(hatch02ID, 2);

				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, texture_hatch03);
				glUniform1i(hatch03ID, 3);

				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, texture_hatch04);
				glUniform1i(hatch04ID, 4);

				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, texture_hatch05);
				glUniform1i(hatch05ID, 5);

				glActiveTexture(GL_TEXTURE6);
				glBindTexture(GL_TEXTURE_2D, depthTexture);
				glUniform1i(shadowMapID, 6);

				glActiveTexture(GL_TEXTURE7);
				glBindTexture(GL_TEXTURE_2D, normalDepthTexture);
				glUniform1i(normalDepthID, 7);
				break;
			case 1:
				//use shader with only hatches (no shadow, no threshold, no outline)
				glUseProgram(programID1);

				//send transformations to currently bound shader in the mvp uniform
				glUniformMatrix4fv(MatrixM1, 1, GL_FALSE, &m[0][0]);
				glUniformMatrix4fv(MatrixV1, 1, GL_FALSE, &v[0][0]);
				glUniformMatrix4fv(MatrixP1, 1, GL_FALSE, &p[0][0]);
				glUniformMatrix4fv(MatrixMV_ti1, 1, GL_FALSE, &mv_ti[0][0]);
				glUniformMatrix4fv(depthMVPID1, 1, GL_FALSE, &depthMVP[0][0]);

				glBindTexture(GL_TEXTURE_2D, texture_hatch00);
				glUniform1i(hatch00ID1, 0);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, texture_hatch01);
				glUniform1i(hatch01ID1, 1);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, texture_hatch02);
				glUniform1i(hatch02ID1, 2);

				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, texture_hatch03);
				glUniform1i(hatch03ID1, 3);

				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, texture_hatch04);
				glUniform1i(hatch04ID1, 4);

				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, texture_hatch05);
				glUniform1i(hatch05ID1, 5);
				break;
			case 2:
				//use shader with only hatches (no shadow, no threshold, no outline)
				glUseProgram(programID2);

				//send transformations to currently bound shader in the mvp uniform
				glUniformMatrix4fv(MatrixM2, 1, GL_FALSE, &m[0][0]);
				glUniformMatrix4fv(MatrixV2, 1, GL_FALSE, &v[0][0]);
				glUniformMatrix4fv(MatrixP2, 1, GL_FALSE, &p[0][0]);
				glUniformMatrix4fv(MatrixMV_ti2, 1, GL_FALSE, &mv_ti[0][0]);
				glUniformMatrix4fv(depthMVPID2, 1, GL_FALSE, &depthMVP[0][0]);

				glBindTexture(GL_TEXTURE_2D, texture_hatch00);
				glUniform1i(hatch00ID2, 0);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, texture_hatch01);
				glUniform1i(hatch01ID2, 1);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, texture_hatch02);
				glUniform1i(hatch02ID2, 2);

				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, texture_hatch03);
				glUniform1i(hatch03ID2, 3);

				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, texture_hatch04);
				glUniform1i(hatch04ID2, 4);

				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, texture_hatch05);
				glUniform1i(hatch05ID2, 5);
				break;
			case 3:
				//use shader with hatch + threshold
				glUseProgram(programID3);

				//send transformations to currently bound shader in the mvp uniform
				glUniformMatrix4fv(MatrixM3, 1, GL_FALSE, &m[0][0]);
				glUniformMatrix4fv(MatrixV3, 1, GL_FALSE, &v[0][0]);
				glUniformMatrix4fv(MatrixP3, 1, GL_FALSE, &p[0][0]);
				glUniformMatrix4fv(MatrixMV_ti3, 1, GL_FALSE, &mv_ti[0][0]);
				glUniformMatrix4fv(depthMVPID3, 1, GL_FALSE, &depthMVP[0][0]);

				glBindTexture(GL_TEXTURE_2D, texture_hatch00);
				glUniform1i(hatch00ID3, 0);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, texture_hatch01);
				glUniform1i(hatch01ID3, 1);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, texture_hatch02);
				glUniform1i(hatch02ID3, 2);

				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, texture_hatch03);
				glUniform1i(hatch03ID3, 3);

				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, texture_hatch04);
				glUniform1i(hatch04ID3, 4);

				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, texture_hatch05);
				glUniform1i(hatch05ID3, 5);

				glActiveTexture(GL_TEXTURE6);
				glBindTexture(GL_TEXTURE_2D, depthTexture);
				glUniform1i(shadowMapID3, 6);

				glActiveTexture(GL_TEXTURE7);
				glBindTexture(GL_TEXTURE_2D, normalDepthTexture);
				glUniform1i(normalDepthID3, 7);
				break;
			case 4:
				//use shader with hatch + threshold
				glUseProgram(programID4);

				//send transformations to currently bound shader in the mvp uniform
				glUniformMatrix4fv(MatrixM4, 1, GL_FALSE, &m[0][0]);
				glUniformMatrix4fv(MatrixV4, 1, GL_FALSE, &v[0][0]);
				glUniformMatrix4fv(MatrixP4, 1, GL_FALSE, &p[0][0]);
				glUniformMatrix4fv(MatrixMV_ti4, 1, GL_FALSE, &mv_ti[0][0]);
				glUniformMatrix4fv(depthMVPID4, 1, GL_FALSE, &depthMVP[0][0]);

				glBindTexture(GL_TEXTURE_2D, texture_hatch00);
				glUniform1i(hatch00ID4, 0);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, texture_hatch01);
				glUniform1i(hatch01ID4, 1);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, texture_hatch02);
				glUniform1i(hatch02ID4, 2);

				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, texture_hatch03);
				glUniform1i(hatch03ID4, 3);

				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, texture_hatch04);
				glUniform1i(hatch04ID4, 4);

				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, texture_hatch05);
				glUniform1i(hatch05ID4, 5);

				glActiveTexture(GL_TEXTURE6);
				glBindTexture(GL_TEXTURE_2D, depthTexture);
				glUniform1i(shadowMapID4, 6);

				glActiveTexture(GL_TEXTURE7);
				glBindTexture(GL_TEXTURE_2D, normalDepthTexture);
				glUniform1i(normalDepthID4, 7);
				break;
			case 9:
				//use shader with only hatches (no shadow, no threshold, no outline)
				glUseProgram(programID9);

				//send transformations to currently bound shader in the mvp uniform
				glUniformMatrix4fv(MatrixM9, 1, GL_FALSE, &m[0][0]);
				glUniformMatrix4fv(MatrixV9, 1, GL_FALSE, &v[0][0]);
				glUniformMatrix4fv(MatrixP9, 1, GL_FALSE, &p[0][0]);
				glUniformMatrix4fv(MatrixMV_ti9, 1, GL_FALSE, &mv_ti[0][0]);
				glUniformMatrix4fv(depthMVPID9, 1, GL_FALSE, &depthMVP[0][0]);

				glBindTexture(GL_TEXTURE_2D, texture_hatch00);
				glUniform1i(hatch00ID9, 0);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, texture_hatch01);
				glUniform1i(hatch01ID9, 1);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, texture_hatch02);
				glUniform1i(hatch02ID9, 2);

				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, texture_hatch03);
				glUniform1i(hatch03ID9, 3);

				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, texture_hatch04);
				glUniform1i(hatch04ID9, 4);

				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, texture_hatch05);
				glUniform1i(hatch05ID9, 5);

				glActiveTexture(GL_TEXTURE6);
				glBindTexture(GL_TEXTURE_2D, depthTexture);
				glUniform1i(shadowMapID9, 6);

				glActiveTexture(GL_TEXTURE7);
				glBindTexture(GL_TEXTURE_2D, normalDepthTexture);
				glUniform1i(normalDepthID9, 7);
				break;
			default:
				break;
		}

		//draw hatched model
		currentModel->render();

		//------------------------------------------------------------------------------ adjust Light Position
		if(glfwGetKey(window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
			phi -= 0.005;
			calcSphereCoords(lightPos, theta, phi, radius);
		}
		if(glfwGetKey(window, GLFW_KEY_LEFT ) == GLFW_PRESS){
			phi += 0.005;
			calcSphereCoords(lightPos, theta, phi, radius);
		}
		if(glfwGetKey(window, GLFW_KEY_UP ) == GLFW_PRESS){
			theta -= 0.005;
			calcSphereCoords(lightPos, theta, phi, radius);
		}
		if(glfwGetKey(window, GLFW_KEY_DOWN ) == GLFW_PRESS){
			theta += 0.005;
			calcSphereCoords(lightPos, theta, phi, radius);
		}

		glUniform3f(lightID, lightPos.x, lightPos.y, lightPos.z);

		//---------------------------------------------------------------------------- render the light bulb to screen
		//use simple shader for light
		glUseProgram(lightProgramID);

		//matrices
		glm::mat4 lightM = glm::translate(glm::mat4(1.0), lightPos);

		//send transformations to currently bound shader in the mvp uniform
		glUniformMatrix4fv(lightMatrixM, 1, GL_FALSE, &lightM[0][0]);
		glUniformMatrix4fv(lightMatrixV, 1, GL_FALSE, &v[0][0]);
		glUniformMatrix4fv(lightMatrixP, 1, GL_FALSE, &p[0][0]);

		ml_lightSphere->render();

		// swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
			&& glfwWindowShouldClose(window) == 0);

	//cleanup VBO
	glDeleteTextures(1, &hatch05ID);
	glDeleteTextures(1, &hatch04ID);
	glDeleteTextures(1, &hatch03ID);
	glDeleteTextures(1, &hatch02ID);
	glDeleteTextures(1, &hatch01ID);
	glDeleteTextures(1, &hatch00ID);
	glDeleteTextures(1, &shadowMapID);
	glDeleteTextures(1, &normalDepthID);
	glDeleteProgram(depthProgramID);
	glDeleteProgram(outlineProgramID);
	glDeleteProgram(programID);
	glDeleteProgram(programID1);
	glDeleteProgram(programID2);
	glDeleteProgram(programID3);
	glDeleteProgram(programID4);
	glDeleteProgram(programID9);

	//close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
