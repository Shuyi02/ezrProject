// Include standard headers
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "utils/ShaderLoad.h"
#include "utils/Control.h"

//not nice but camera mouse controls work
GLFWwindow* window;

// An array of 3 vectors which represents 3 vertices
static const GLfloat g_vertex_buffer_data[] = {

   -1.0f, -1.0f, 0.0f,

   1.0f, -1.0f, 0.0f,

   0.0f,  1.0f, 0.0f

};

// An array of 3 vectors which represents 3 vertices
static const GLfloat g_vertex_buffer_data_normals[] = {
//		   -1.0f, -1.0f, 1.0f,
//		   1.0f, -1.0f, 1.0f,
//		   0.0f,  1.0f, 1.0f
		   0.0f, 0.0f, 1.0f,
		   0.0f, 0.0f, 1.0f,
		   0.0f, 0.0f, 1.0f
};

int main() {

	//----------------------------------------------------------------setup
	 // initialize GLFW
	 if( !glfwInit() )	 {
	     fprintf( stderr, "Failed to initialize GLFW\n" );
	     return -1;
	 }

	 glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	 glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // we want OpenGL 3.3
	 glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	 glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //we don't want the old OpenGL

	 // Open a window and create its OpenGL context
	 window = glfwCreateWindow( 1024, 768, "myEZR", NULL, NULL);

	 if( window == NULL ){
	     fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
	     glfwTerminate();
	     return -1;
	 }

	 // initialize GLEW
	 glfwMakeContextCurrent(window);
	 glewExperimental=true; // needed in core profile
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
	glfwSetCursorPos(window, 1024/2, 768/2);

	// dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	 //--------------------------------------------------------------- Vertex Buffer
	 // Vertex Array Object
	 GLuint VertexArrayID;
	 glGenVertexArrays(1, &VertexArrayID);
	 glBindVertexArray(VertexArrayID);


	 // creating buffer
	 GLuint vertexbufferPos;
	 glGenBuffers(1, &vertexbufferPos);
	 glBindBuffer(GL_ARRAY_BUFFER, vertexbufferPos);
	 // give our vertices to OpenGL.
	 glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	//		glEnableVertexAttribArray(1);   //vero look for vertex[0] normale[1]
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferPos);
	glVertexAttribPointer(
	//		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	0,                  // attribute 1. -> must match the layout in the shader. vero have to looking for
	3,                  // size
	GL_FLOAT,           // type
	GL_FALSE,           // normalized?
	0,                  // stride
	(void*)0            // array buffer offset
	);

	 // creating buffer normals
	 GLuint vertexbufferNor;
	 glGenBuffers(1, &vertexbufferNor);
	 glBindBuffer(GL_ARRAY_BUFFER, vertexbufferNor);
	 // give our vertices to OpenGL.
	 glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_normals), g_vertex_buffer_data_normals, GL_STATIC_DRAW);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
	//		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	1,                  // attribute 1. -> must match the layout in the shader. vero have to looking for
	3,                  // size
	GL_FLOAT,           // type
	GL_FALSE,           // normalized?
	0,                  // stride
	(void*)0            // array buffer offset
	);

	 // ------------------------------------------------------------- create and compile GLSL program from shaders
	 GLuint programID = utils::loadShaders( SHADERS_PATH "/minimal.vert",SHADERS_PATH "/minimal.frag" );

	 //-------------------------------------------------------------- Uniforms
	 //handle for our "mvp" modelViwProjMatrix uniform
	 GLuint MatrixID = glGetUniformLocation(programID, "mvp");
	 GLuint MatrixIDMV = glGetUniformLocation(programID, "mv");
	 GLuint MatrixIDTIMV = glGetUniformLocation(programID, "ti_mv");

	// ---------------------------------------------------------------rendering loop
	do{

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//use shader
		glUseProgram(programID);

		 //---------------------------------------- ModelViewProjection
		utils::computeMatricesFromInputs();
		glm::mat4 projection = utils::getProjectionMatrix();
		glm::mat4 view = utils::getViewMatrix();
		glm::mat4 model = glm::mat4(1.0);
		glm::mat4 mvp = projection * view * model;
		glm::mat4 mv = view * model;
		glm::mat4 ti_mv = glm::transpose(glm::inverse(mv));
		//send transformation to the currently bound shader in the mvp uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
		glUniformMatrix4fv(MatrixIDMV, 1, GL_FALSE, &mv[0][0]);
		glUniformMatrix4fv(MatrixIDTIMV, 1, GL_FALSE, &ti_mv[0][0]);


		// ------------------------------------------------------ draw
		glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle


		// swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0 );

	//cleanup VBO
	glDeleteBuffers(1, &vertexbufferPos);
	glDeleteBuffers(1, &vertexbufferNor);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	//close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
