#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
const int width = 800;
const int height = 600;

double aspect = (double) width / height;
GLFWwindow* window;

glm::mat4 view, proj;

int main() {

	 // Initialise GLFW

	 if( !glfwInit() )

	 {

	     fprintf( stderr, "Failed to initialize GLFW\n" );

	     return -1;

	 }

	 glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing

	 glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3

	 glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	 glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed

	 glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL



	 // Open a window and create its OpenGL context

	 GLFWwindow* window; // (In the accompanying source code, this variable is global)

	 window = glfwCreateWindow( 1024, 768, "Tutorial 01", NULL, NULL);

	 if( window == NULL ){

	     fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );

	     glfwTerminate();

	     return -1;

	 }

	 glfwMakeContextCurrent(window); // Initialize GLEW

	 glewExperimental=true; // Needed in core profile

	 if (glewInit() != GLEW_OK) {

	     fprintf(stderr, "Failed to initialize GLEW\n");

	     return -1;

	 }


	 // Ensure we can capture the escape key being pressed below

	 glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);



	 do{

	     // Draw nothing, see you in tutorial 2 !



	     // Swap buffers

	     glfwSwapBuffers(window);

	     glfwPollEvents();



	 } // Check if the ESC key was pressed or the window was closed

	 while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&

	 glfwWindowShouldClose(window) == 0 );
//	view = glm::lookAt(glm::vec3(0, 15, 30), glm::vec3(0, 15, 0),
//			glm::vec3(0, 1, 0));
//	proj = glm::perspective(60.0, aspect, 0.1, 1000.0);
//
//	//Init Window
//	glfwInit();
//
//	window = glfwCreateWindow(width, height, "RenderTools", NULL, NULL);
//	glfwSetWindowPos(window, 600, 50);
//	glfwMakeContextCurrent(window);
//
//	glewInit();
//
//	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
//
//	glEnable(GL_DEPTH_TEST);
//	glDepthMask(GL_TRUE);
//	glDepthFunc(GL_LEQUAL);
//	glDepthRange(0.0f, 1.0f);
//
//	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//	glClearDepth(1.0f);
//	int depth;
//	glGetIntegerv(GL_DEPTH_BITS, &depth);
//	std::cout << std::endl << depth << " bits depth" << std::endl;
//	while (!glfwWindowShouldClose(window)) {
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		glfwSwapBuffers(window);
//	}
//
//	glfwDestroyWindow(window);
//	glfwTerminate();
//	return 0;
}
