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

#include "utils/Control.h"
#include "utils/Model.h"
#include "utils/ShaderLoad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "utils/stb_image.h"

#include "utils/TextureManager.h"

//not nice but camera mouse controls work
GLFWwindow* window;

int main() {

	// --------------------------------------------------------------- setup
	// initialize GLFW
	if (!glfwInit()) {
		fprintf( stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	Eigen::Matrix3d nyah = Eigen::Matrix3d::Identity();
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // we want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //we don't want the old OpenGL

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "myEZR", NULL, NULL);

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

	// ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// dark blue background
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// --------------------------------------------------------------------- Load Model and Texture

	utils::Model *ml_ogre = new utils::Model(RESOURCES_PATH "/Models/imrod/ImrodLowPoly.obj");
	utils::Model *ml_sphere = new utils::Model(RESOURCES_PATH "/Models/sphere/sphere.obj");
	utils::Model *ml_lightSphere = new utils::Model(RESOURCES_PATH "/Models/sphere/lightOrb.obj");
	utils::Model *ml_suzanne = new utils::Model(RESOURCES_PATH "/Models/suzanne/suzanne.obj");
	utils::Model *ml_teapot = new utils::Model(RESOURCES_PATH "/Models/teapot/pot.obj");
	utils::Model *ml_tetris= new utils::Model(RESOURCES_PATH "/Models/tetris/tetris.obj");
	utils::Model *ml_torus= new utils::Model(RESOURCES_PATH "/Models/torus/torus.obj");
	utils::Model *currentModel = ml_teapot;

	GLuint texture_kitty = utils::loadTexture(RESOURCES_PATH"/kitty.jpg");

	std::string texturePath = RESOURCES_PATH"/hatchesLess02/hatch";
//	std::string texturePath = RESOURCES_PATH"/dots01/dots";
	int mipMapLevel = 1;
	GLuint texture_hatch00 = utils::loadMipMapTexture(texturePath+"0", mipMapLevel);
	GLuint texture_hatch01 = utils::loadMipMapTexture(texturePath+"1", mipMapLevel);
	GLuint texture_hatch02 = utils::loadMipMapTexture(texturePath+"2", mipMapLevel);
	GLuint texture_hatch03 = utils::loadMipMapTexture(texturePath+"3", mipMapLevel);
	GLuint texture_hatch04 = utils::loadMipMapTexture(texturePath+"4", mipMapLevel);
	GLuint texture_hatch05 = utils::loadMipMapTexture(texturePath+"5", mipMapLevel);


	//----------------------------------------------------------------------- shaders for shadowMap
	GLuint depthProgramID = utils::loadShaders( SHADERS_PATH "/depthShadow.vert", SHADERS_PATH "/depthShadow.frag" );

	// Get a handle for our "MVP" uniform
	GLuint depthMatrixID = glGetUniformLocation(depthProgramID, "depthMVP");

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	GLuint FramebufferShadow = 1;
	glGenFramebuffers(1, &FramebufferShadow);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferShadow);


	float miau[4] = {1.0f, 1.0f, 1.0f, 1.0f}; //kann weg oder //texturrand-Farbe da clamp to border

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	GLuint depthTexture;
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, 1024, 1024, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //nimm naechstliegenden pixel beim up scaling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //nimm naechstliegenden pixel beim down scaling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); //clamp in u richtung
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); //clamp in v richtung
//	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, miau); //border color for clamp
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL); //interpolierte Texturkoordinate kleiner gleich Tiefentexturwert, der von der aktuell gebundenen Tiefentextur dann 1 sonst 0. Das Ergebnis wird entweder der Luminanz, der Intensitaet oder Alpha zugewiesen
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	glDrawBuffer(GL_NONE); // No color buffer is drawn to.


	//------------------------------------------------------------------------------ shaders for outline
	GLuint outlineProgramID = utils::loadShaders( SHADERS_PATH "/preOutline.vert", SHADERS_PATH "/preOutline.frag" );
	glUseProgram(outlineProgramID);

	// Get a handle for our "MVP" uniform
	GLuint outlineMatrixID = glGetUniformLocation(outlineProgramID, "normalDepth");
	//	GLuint normalDepthID = glGetUniformLocation(normalDepthProgramID, "normalDepth");

	glUseProgram(0);

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	GLuint FramebufferOutline = 1;
	glGenFramebuffers(1, &FramebufferOutline);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferOutline);

	//vero try texture7
	GLuint normalDepthTexture;
	glGenTextures(1, &normalDepthTexture);
	glBindTexture(GL_TEXTURE_2D, normalDepthTexture);
	float redBorder[4] = {1.0f, 0.0f, 0.0f, 1.0f}; //textur-Randfarbe da clamp to border
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, redBorder); //textur-Randfarbe da clamp to border
	//Eigenschaften
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_FLOAT, 0); //RGBA 4 Parameter, Aufloesung 1024x1024 als Angabe fuer Texture2D
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); //wrapping: repeat,clamp,.. in u Richtung
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); //wrapping: repeat,clamp,.. in v Richtung
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //nimmt naechsten Pixelwert und interpoliert nich von den 4 nachbarn
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // da wir Aufoesung entsprechend Tex angelegt haben sollte das passen
	//
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, normalDepthTexture, 0);


	 // The depth buffer
	 GLuint depthrenderbuffer;
	 glGenRenderbuffers(1, &depthrenderbuffer);
	 glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	 glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 1024);
	 glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);


	 // Set "renderedTexture" as our colour attachement #0
	 glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, normalDepthTexture, 0);

	 // Set the list of draw buffers.
	 GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	 glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers


//	 //-------------------- Framebuffer2 for soble ----------------
//	 // FramebufferSoble, sobelProgramID, sobelTexture
//	 GLuint sobelProgramID = utils::loadShaders( SHADERS_PATH "/outline.vert", SHADERS_PATH "/outline.frag" );
//	 glUseProgram(sobelProgramID);
//	 glUseProgram(0);
//	 // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
//	 GLuint FramebufferSobel = 1;
//	 glGenFramebuffers(1, &FramebufferSobel);
//	 glBindFramebuffer(GL_FRAMEBUFFER, FramebufferSobel);
//
//	GLuint sobelTexture;
//	glGenTextures(1, &sobelTexture);
//	glBindTexture(GL_TEXTURE_2D, sobelTexture);
//	float blueBorder[4] = {0.0f, 0.0f, 1.0f, 1.0f};
//	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, blueBorder);
//	 	//Eigenschaften
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_FLOAT, 0);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, normalDepthTexture, 0); // Set "renderedTexture" as our colour attachement #0
////	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 }; // Set the list of draw buffers.
//	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
	//------------------------------------------ veros spielwiese ende ----------------------------------------------


	// always check framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	return false;

	// --------------------------------------------------------------- create and compile main shaders for hatching
	GLuint programID = utils::loadShaders( SHADERS_PATH "/hatch.vert",
		SHADERS_PATH "/hatch.frag");

	//---------------------------------------------------------------- lightPosition
	glm::vec3 lightPos = glm::vec3(-8.0f, 6.0f, 0.0f);
	GLuint lightID = glGetUniformLocation(programID, "lightPos_Model");

	// --------------------------------------------------------------- Uniforms
	//handle for our "mvp" modelViwProjMatrix uniform
	GLuint MatrixM = glGetUniformLocation(programID, "m");
	GLuint MatrixV = glGetUniformLocation(programID, "v");
	GLuint MatrixP = glGetUniformLocation(programID, "p");
	GLuint MatrixMV_ti = glGetUniformLocation(programID, "MatrixMV_ti");
	GLuint depthMVPID = glGetUniformLocation(programID, "depthMVP");
	GLuint ShadowMapID = glGetUniformLocation(programID, "shadowMap");
//	//vero try outline
	GLuint normalDepthID = glGetUniformLocation(programID, "normalDepth");


	//handle for our "textureSampler" uniform
	GLuint hatch00ID = glGetUniformLocation(programID, "hatch00");
	GLuint hatch01ID = glGetUniformLocation(programID, "hatch01");
	GLuint hatch02ID = glGetUniformLocation(programID, "hatch02");
	GLuint hatch03ID = glGetUniformLocation(programID, "hatch03");
	GLuint hatch04ID = glGetUniformLocation(programID, "hatch04");
	GLuint hatch05ID = glGetUniformLocation(programID, "hatch05");

	//---------------------------------------------------------------- shaders for simple lightbulb
	GLuint lightProgramID = utils::loadShaders( SHADERS_PATH "/minimal.vert",
	SHADERS_PATH "/minimal.frag");

	GLuint lightMatrixM = glGetUniformLocation(lightProgramID, "m");
	GLuint lightMatrixV = glGetUniformLocation(lightProgramID, "v");
	GLuint lightMatrixP = glGetUniformLocation(lightProgramID, "p");

	// --------------------------------------------------------------- rendering loop
	do {
		// --------------------------------------------------------------------------  Render to texture
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferShadow);
		glViewport(0,0,1024,1024);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// use shader for depth
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
							0.5, 0.5, 0.5, 1.0
						);
				glm::mat4 depthBiasMVP = biasMatrix*depthMVP;

		//------------------------------------------------------------------------------ Render to texture veros spielwiese
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferOutline); //vero try maybe
//		glBindFramebuffer(GL_FRAMEBUFFER, 0); //vero try maybe
		glViewport(0,0,1024,1024); //render from 0;0 bis 1024;1024 hole framebuffer


//		glEnable(GL_CULL_FACE);
//		glCullFace(GL_BACK);
//
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(outlineProgramID);


		glm::mat4 normalDepthMVP = p*v*m;
		//vero try outline need a transformation
		glUniformMatrix4fv(outlineMatrixID, 1, GL_FALSE, &normalDepthMVP[0][0]);

		//draw model
		currentModel->render();

//		//------- veros zweiter teil -------
//		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferSobel);
//		glViewport(0,0,1024,1024);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glUseProgram(sobelProgramID);
////		glm::mat4 normalDepthMVP = p*v*m; //?
////		glUniformMatrix4fv(outlineMatrixID, 1, GL_FALSE, &normalDepthMVP[0][0]);
//		currentModel->render();

		//--------------------------------------------- veros spielwiese ende ---------------------------------------------

		//---------------------------------------------------------------------------- render to the screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0,0,1024, 768);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//use main shader
		glUseProgram(programID);



		//render from view of the light
		if(glfwGetKey(window, GLFW_KEY_K ) == GLFW_PRESS){
			v = depthViewMatrix;
			p = depthProjectionMatrix;
		}

		//send transformations to currently bound shader in the mvp uniform
		glUniformMatrix4fv(MatrixM, 1, GL_FALSE, &m[0][0]);
		glUniformMatrix4fv(MatrixV, 1, GL_FALSE, &v[0][0]);
		glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &p[0][0]);
		glUniformMatrix4fv(MatrixMV_ti, 1, GL_FALSE, &mv_ti[0][0]);
		glUniformMatrix4fv(depthMVPID, 1, GL_FALSE, &depthMVP[0][0]);



		//---------------------------------------- Texture
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
		glUniform1i(ShadowMapID, 6);

		//---vero try outline -- normale u tiefe aus Kamerasicht
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, normalDepthTexture);
		glUniform1i(normalDepthID, 7);

		//draw hatched model
		currentModel->render();

		//------------------------------------------------ Light Position
//		lightPos.x += 0.01;
//		//light pos to shader
//		if(lightPos.x >= 8.0){
//			lightPos.x = -8.0;
//		}
		if(glfwGetKey(window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
			lightPos.x += 0.01;
		}
		if(glfwGetKey(window, GLFW_KEY_LEFT ) == GLFW_PRESS){
			lightPos.x -= 0.01;
		}
		if(glfwGetKey(window, GLFW_KEY_UP ) == GLFW_PRESS){
			lightPos.y += 0.01;
		}
		if(glfwGetKey(window, GLFW_KEY_DOWN ) == GLFW_PRESS){
			lightPos.y -= 0.01;
		}
		if(glfwGetKey(window, GLFW_KEY_N ) == GLFW_PRESS){
			lightPos.z -= 0.01;
		}
		if(glfwGetKey(window, GLFW_KEY_M ) == GLFW_PRESS){
			lightPos.z += 0.01;
		}
		glUniform3f(lightID, lightPos.x, lightPos.y, lightPos.z);

		//---------------------------------------------------------------------------- render the light bulb
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
	glDeleteProgram(depthProgramID);
	//vero try outline
	glDeleteProgram(outlineProgramID);
//	glDeleteProgram(sobelProgramID);
	glDeleteProgram(programID);

	//close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
