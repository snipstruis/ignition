#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> // for strlen()
#include <math.h>
#include <unistd.h>

int render_mandelline = 0;

#include "uniforms.h"
#include "utils.h"

#include "callbacks.h"

int min(int a,int b){return a<b?a:b;}
int max(int a,int b){return a>b?a:b;}

void window_transform(double* cx, double* cy,int sx,int sy){
	*cx = ((*cx/(double)sx)* 2) -1;
	*cy = ((*cy/(double)sy)*-2) +1;
}

void world_transform(double* cx, double* cy,int sx,int sy){
	if(sx>sy){
		*cx = (*cx/uni_scale) - uni_scale*uni_center_x*(sy/(double)sx);
		*cy = (*cy/uni_scale) - uni_scale*uni_center_y;
	}else{
		*cx = (*cx/uni_scale) - uni_scale*uni_center_x;
		*cy = (*cy/uni_scale) - uni_scale*uni_center_y*(sx/(double)sy);
	}
}

void inv_world_transform(double* cx, double* cy,int sx,int sy){
	if(sx>sy){
		*cx = uni_scale*(*cx+uni_scale*uni_center_x*(sy/(double)sx));
		*cy = uni_scale*(*cy+uni_scale*uni_center_y);
	}else{
		*cx = uni_scale*(*cx+uni_scale*uni_center_x);
		*cy = uni_scale*(*cy+uni_scale*uni_center_y*(sx/(double)sy));
	}
}

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
	window = glfwCreateWindow(800, 600, "floating_window", NULL, NULL);

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
	char* file = read_file("../interactive-mandel-julia/float.frag");
	GLint prog = use_shader(file);
	free(file);

	init_uniforms(prog);

	/* set initial screen size and center mouse*/{
		int w,h;
		glfwGetFramebufferSize(window, &w, &h);
		framebuffer_size_callback(window,w,h);
		glfwSetCursorPos(window,w/2.0,h/2.0);
	}

	// draw a screen-filling rectangle

	while (!glfwWindowShouldClose(window)){
		// clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

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

		if(render_mandelline){
			// revert to fixed-function pipeline to prevent the shader from
			// coloring the line in the same way as the background
			glUseProgram(0);

			glBegin(GL_LINE_STRIP);
				glLineWidth(2.5);
				glColor3f(1.f,1.f,1.f);

				double x,y;
				glfwGetCursorPos(window,&x,&y);
				window_transform(&x,&y,sx,sy);
				glVertex3f((float)x,(float)y,0);

				x=0; y=0;
				world_transform(&x,&y,sx,sy);
				glVertex3f((float)x,(float)y,0);
			glEnd();

			// re-enable the shader
			glUseProgram(prog);
		}

		if(1){
			glUseProgram(0);

			double overlaySizeX = 300,
				   overlaySizeY = 200;

			double xmin = (sx/2)-(overlaySizeX/2),
				   ymin = (sy/2)-(overlaySizeY/2),
				   xmax = (sx/2)+overlaySizeX/2,
				   ymax = (sy/2)+overlaySizeY/2;

			window_transform(&xmin,&ymin,sx,sy);
			window_transform(&xmax,&ymax,sx,sy);

			glBegin(GL_QUADS);
				glColor3f(0.f,1.f,1.f);
				glVertex3f(xmin, ymin, 0.f);
				glVertex3f(xmin, ymax, 0.f);
				glVertex3f(xmax, ymax, 0.f);
				glVertex3f(xmax, ymin, 0.f);
			glEnd();

			glUseProgram(prog);
		}

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
