#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> // for strlen()
#include <math.h>
#include <unistd.h>

#include "uniforms.h"
#include "utils.h"

#include "callbacks.h"

int main(void){
	// create windowhandle
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	// initialize glfw
	if (!glfwInit()) exit(EXIT_FAILURE);
	else printf("GLFW: succesfully initialized\n");

	// create window with an OpenGL 2.0 context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,0);
	window = glfwCreateWindow(800, 600, "sfml", NULL, NULL);

	// check if window has been created
	if (!window){ glfwTerminate(); exit(EXIT_FAILURE); }
	else printf("GLFW: window succesfully  created\n");

	// assign context to current thread
	glfwMakeContextCurrent(window);

	// set callbacks
	init_callbacks(window);

	// initialise GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err){
		fprintf(stderr, "GLEW: %s\n", glewGetErrorString(err));
		exit(EXIT_FAILURE);
	} else printf("GLEW: succesfully initialized\n");

	// load shader
	char* file = read_file("../interactive/float.frag");
	GLint prog = use_shader(file);
	free(file);

	init_uniforms(prog);

	/* set initial screen size */{
		int w,h;
		glfwGetFramebufferSize(window, &w, &h);
		framebuffer_size_callback(window,w,h);
	}

	while (!glfwWindowShouldClose(window)){
		// clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// draw a screen-filling rectangle
		glBegin(GL_QUADS);
		glVertex3f(-1.f, -1.f, 0.f);
		glVertex3f(-1.f,  1.f, 0.f);
		glVertex3f( 1.f,  1.f, 0.f);
		glVertex3f( 1.f, -1.f, 0.f);
		glEnd();

		double cx,cy;
		glfwGetCursorPos(window,&cx,&cy);

		int sx,sy;
		glfwGetFramebufferSize(window,&sx,&sy);

		edgescroll(cx,cy,sx,sy);
		if(uni_mode == 0) update_julia((float)cx,(float)cy,(float)sx,(float)sy);

		// display what is drawn
		glfwSwapBuffers(window);

		// get events
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
