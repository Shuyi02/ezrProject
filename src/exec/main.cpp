// Include standard headers
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Eigen/Dense>
#include <Eigen/Core>

#include <iostream>
using namespace std;

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

	GLuint texture_kitty = utils::loadTexture(RESOURCES_PATH"/kitty.jpg");

//	GLuint texture_hatch00 = utils::loadMipMapTexture(RESOURCES_PATH"/hatches/hatch0", 3);
//	GLuint texture_hatch01 = utils::loadMipMapTexture(RESOURCES_PATH"/hatches/hatch1", 3);
//	GLuint texture_hatch02 = utils::loadMipMapTexture(RESOURCES_PATH"/hatches/hatch2", 3);
//	GLuint texture_hatch03 = utils::loadMipMapTexture(RESOURCES_PATH"/hatches/hatch3", 3);
//	GLuint texture_hatch04 = utils::loadMipMapTexture(RESOURCES_PATH"/hatches/hatch4", 3);
//	GLuint texture_hatch05 = utils::loadMipMapTexture(RESOURCES_PATH"/hatches/hatch5", 3);
	GLuint texture_hatch00 = utils::loadMipMapTexture(RESOURCES_PATH"/hatchesPaper/0", 3);
	GLuint texture_hatch01 = utils::loadMipMapTexture(RESOURCES_PATH"/hatchesPaper/1", 3);
	GLuint texture_hatch02 = utils::loadMipMapTexture(RESOURCES_PATH"/hatchesPaper/2", 3);
	GLuint texture_hatch03 = utils::loadMipMapTexture(RESOURCES_PATH"/hatchesPaper/3", 3);
	GLuint texture_hatch04 = utils::loadMipMapTexture(RESOURCES_PATH"/hatchesPaper/4", 3);
	GLuint texture_hatch05 = utils::loadMipMapTexture(RESOURCES_PATH"/hatchesPaper/5", 3);
//	GLuint texture_hatch00 = utils::loadMipMapTexture(RESOURCES_PATH"/hatchTest/hatch_0", 4);
//	GLuint texture_hatch01 = utils::loadMipMapTexture(RESOURCES_PATH"/hatchTest/hatch_1", 4);
//	GLuint texture_hatch02 = utils::loadMipMapTexture(RESOURCES_PATH"/hatchTest/hatch_2", 4);
//	GLuint texture_hatch03 = utils::loadMipMapTexture(RESOURCES_PATH"/hatchTest/hatch_3", 4);
//	GLuint texture_hatch04 = utils::loadMipMapTexture(RESOURCES_PATH"/hatchTest/hatch_4", 4);
//	GLuint texture_hatch05 = utils::loadMipMapTexture(RESOURCES_PATH"/hatchTest/hatch_5", 4);


	//----------------------------------------------------------------------- shaders for shadowMap
	GLuint depthProgramID = utils::loadShaders( SHADERS_PATH "/depthShadow.vert", SHADERS_PATH "/depthShadow.frag" );

	// Get a handle for our "MVP" uniform
	GLuint depthMatrixID = glGetUniformLocation(depthProgramID, "depthMVP");

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	GLuint FramebufferShadow = 1;
	glGenFramebuffers(1, &FramebufferShadow);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferShadow);

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	GLuint depthTexture;
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, 1024, 768, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	glDrawBuffer(GL_NONE); // No color buffer is drawn to.

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
	GLuint MatrixMV_ti = glGetUniformLocation(programID, "mv_ti");
	GLuint depthMVPID = glGetUniformLocation(programID, "depthMVP");
	GLuint ShadowMapID = glGetUniformLocation(programID, "shadowMap");

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
		glViewport(0,0,1024,768);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// use shader for depth
		glUseProgram(depthProgramID);

		glm::mat4 depthProjectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
		glm::mat4 depthViewMatrix = glm::lookAt(lightPos, glm::vec3(0,0,0), glm::vec3(0,1,0));
		glm::mat4 depthModelMatrix = glm::mat4(1.0);
		glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

		// send transformation to the currently bound shader
		glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);

		ml_teapot->render();

		//---------------------------------------------------------------------------- render to the screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0,0,1024, 768);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//use main shader
		glUseProgram(programID);

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

		//draw hatched model
		ml_teapot->render();

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
		utils::computeMatricesFromInputs();
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
	glDeleteProgram(programID);

	//close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
