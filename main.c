#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>
#include <SOIL/SOIL.h>

bool render_mandelline = false;
enum{
	OVERLAY_FULLSCREEN,
	OVERLAY_INTRODUCTION,
	OVERLAY_INFORMATION,
	OVERLAY_MAX
} overlay = OVERLAY_FULLSCREEN;

#include "uniforms.h"
#include "utils.h"
#include "callbacks.h"
#include "transform.h"

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
	window = glfwCreateWindow(800, 600, "Ignition", NULL, NULL);

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
	char* file = read_file("../ignition/float.frag");
	GLint prog = use_shader(file);
	free(file);

	init_uniforms(prog);

	/* set initial screen size and center mouse*/{
		int w,h;
		glfwGetFramebufferSize(window, &w, &h);
		framebuffer_size_callback(window,w,h);
		glfwSetCursorPos(window,w/2.0,h/2.0);
	}

	// load texture using SOIL
	GLuint texture[OVERLAY_MAX];

	texture[OVERLAY_FULLSCREEN] =
		SOIL_load_OGL_texture(
			"../ignition/fullscreen.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			0
		);

	texture[OVERLAY_INTRODUCTION] =
		SOIL_load_OGL_texture(
			"../ignition/introduction.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			0
		);

	texture[OVERLAY_INFORMATION] =
		SOIL_load_OGL_texture(
			"../ignition/information.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			0
		);

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

		// lines
		if(render_mandelline){
			// revert to fixed-function pipeline to prevent the shader from
			// coloring the line in the same way as the background
			glUseProgram(0);

			glBegin(GL_LINE_STRIP);
				glLineWidth(1);
				glColor3f(1.f,1.f,1.f);

				const int maxiter = 100;
				double x, y, my = 0.0, mx = 0.0, mxt, rx, ry;
				glfwGetCursorPos(window,&x,&y);
				pixel_render_transform(&x,&y,sx,sy);
				glVertex3f((float)x,(float)y,0);

				render_world_transform(&x, &y, sx, sy);

				for(int i=0; i<maxiter; i++){
					mxt = mx*mx - my*my + x;
					my  = 2.0*mx*my + y;
					mx  = mxt;
					rx = mx;
					ry = my;
					world_render_transform(&rx,&ry,sx,sy);
					glVertex3f((float)rx,(float)ry,0);
				}
			glEnd();

			// re-enable the shader
			glUseProgram(prog);
		}

		// capture screenshot here to avoid also capturing the overlay
		if (glfwGetKey(window,GLFW_KEY_Q)==GLFW_PRESS){
			char filename[50];
			sprintf(filename,"screenshot_%d.tga",time());
			printf("saving screenshot to \"%s\" ...",filename);
			screenshot(window,filename);
			printf("done!\n");
		}

		/* draw overlay */{
			int w, h;
			glBindTexture(GL_TEXTURE_2D, texture[overlay]);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT,&h);

			// allign to bottom right
			double xmin = sx-(double)w,
				   ymin = sy-(double)h,
				   xmax = sx,
				   ymax = sy;
			pixel_render_transform(&xmin,&ymin,sx,sy);
			pixel_render_transform(&xmax,&ymax,sx,sy);

			glUseProgram(0);
			glEnable(GL_TEXTURE_2D);

			glBegin(GL_QUADS);
				glTexCoord2f(0, 0); glVertex3f(xmin, ymin, 0.f);
				glTexCoord2f(0, 1); glVertex3f(xmin, ymax, 0.f);
				glTexCoord2f(1, 1); glVertex3f(xmax, ymax, 0.f);
				glTexCoord2f(1, 0); glVertex3f(xmax, ymin, 0.f);
			glEnd();

			glDisable(GL_TEXTURE_2D);
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
