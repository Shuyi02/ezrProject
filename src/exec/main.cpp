// Include standard headers
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

	// --------------------------------------------------------------- Load Model and Texture

	utils::Model *ml_ogre = new utils::Model(RESOURCES_PATH "/Models/imrod/ImrodLowPoly.obj");
	utils::Model *ml_sphere = new utils::Model(RESOURCES_PATH "/Models/sphere/sphere.obj");
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
	GLuint texture_hatch00 = utils::loadMipMapTexture(RESOURCES_PATH"/hatchTest/hatch_0", 4);
	GLuint texture_hatch01 = utils::loadMipMapTexture(RESOURCES_PATH"/hatchTest/hatch_1", 4);
	GLuint texture_hatch02 = utils::loadMipMapTexture(RESOURCES_PATH"/hatchTest/hatch_2", 4);
	GLuint texture_hatch03 = utils::loadMipMapTexture(RESOURCES_PATH"/hatchTest/hatch_3", 4);
	GLuint texture_hatch04 = utils::loadMipMapTexture(RESOURCES_PATH"/hatchTest/hatch_4", 4);
	GLuint texture_hatch05 = utils::loadMipMapTexture(RESOURCES_PATH"/hatchTest/hatch_5", 4);

	// --------------------------------------------------------------- create and compile GLSL program from shaders
	GLuint programID = utils::loadShaders( SHADERS_PATH "/minimal.vert",
	SHADERS_PATH "/minimal.frag");


	//---------------------------------------------------------------- lightPosition
	glm::vec3 lightPos = glm::vec3(-5.0f, 6.0f, 6.0f);
	GLuint lightID = glGetUniformLocation(programID, "lightPos_Model");

	// --------------------------------------------------------------- Uniforms
	//handle for our "mvp" modelViwProjMatrix uniform
	GLuint MatrixM = glGetUniformLocation(programID, "m");
	GLuint MatrixV = glGetUniformLocation(programID, "v");
	GLuint MatrixP = glGetUniformLocation(programID, "p");
	GLuint MatrixMV_ti = glGetUniformLocation(programID, "mv_ti");

	//handle for our "textureSampler" uniform
	GLuint mipMapID = glGetUniformLocation(programID, "mipMap");

	//handle for our "textureSampler" uniform
	GLuint hatch00ID = glGetUniformLocation(programID, "hatch00");
	GLuint hatch01ID = glGetUniformLocation(programID, "hatch01");
	GLuint hatch02ID = glGetUniformLocation(programID, "hatch02");
	GLuint hatch03ID = glGetUniformLocation(programID, "hatch03");
	GLuint hatch04ID = glGetUniformLocation(programID, "hatch04");
	GLuint hatch05ID = glGetUniformLocation(programID, "hatch05");

	glm::vec3 test = glm::vec3(-8.0f, 8.0f, 6.0f);

	// --------------------------------------------------------------- rendering loop
	do {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//use shader
		glUseProgram(programID);

		//---------------------------------------- Matrices
		utils::computeMatricesFromInputs();
		glm::mat4 m = glm::mat4(1.0);
		glm::mat4 v = utils::getViewMatrix();
		glm::mat4 p = utils::getProjectionMatrix();
		glm::mat4 mv_ti = glm::transpose(glm::inverse(v*m));

		//send transformation to the currently bound shader in the mvp uniform
		glUniformMatrix4fv(MatrixM, 1, GL_FALSE, &m[0][0]);
		glUniformMatrix4fv(MatrixV, 1, GL_FALSE, &v[0][0]);
		glUniformMatrix4fv(MatrixP, 1, GL_FALSE, &p[0][0]);
		glUniformMatrix4fv(MatrixMV_ti, 1, GL_FALSE, &mv_ti[0][0]);

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

		//---------------------------------------- Texture
		// Bind our texture in Texture Unit 0
		glBindTexture(GL_TEXTURE_2D, texture_hatch00);
		// Set our "textureSampler" sampler to user Texture Unit 0
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


		//---------------------------------------- draw (switch between triangle and model)

		ml_torus->render();

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
	glDeleteTextures(1, &mipMapID);
	glDeleteProgram(programID);

	//close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
