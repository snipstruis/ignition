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

void aspectratio(double* arx, double* ary){
	double x = *arx, y = *ary;
	*arx /= fmin(x, y);
	*ary /= fmin(x, y);
}

void pixel_render_transform(double* cx, double* cy,int sx,int sy){
	*cy = -*cy;
	*cx = ((2.0 * *cx) / (double)sx) - 1;
	*cy = ((2.0 * *cy) / (double)sy) + 1;
}
void render_pixel_transform(double* cx, double* cy,int sx,int sy){
	*cy = -*cy;
	*cx = (*cx + 1) * ((double)sx / 2.0);
	*cy = (*cy + 1) * ((double)sy / 2.0);
}
void render_world_transform(double* cx, double* cy,int sx,int sy){
	double arx = (double)sx, ary = (double)sy;
	aspectratio(&arx,&ary);
	//*cy = -*cy;
	*cx = ((*cx * arx) / uni_scale) + uni_center_x;
	*cy = ((*cy * ary) / uni_scale) + uni_center_y;
}
void world_render_transform(double* cx, double* cy,int sx,int sy){
	double arx = (double)sx, ary = (double)sy;
	aspectratio(&arx,&ary);
	//*cy = -*cy;
	*cx = ((*cx - uni_center_x) * uni_scale) / arx;
	*cy = ((*cy - uni_center_y) * uni_scale) / ary;
}
void pixel_world_transform(double* cx, double* cy,int sx,int sy){
	pixel_render_transform(cx,cy,sx,sy);
	render_world_transform(cx,cy,sx,sy);
}
void world_pixel_transform(double* cx, double* cy,int sx,int sy){
	world_render_transform(cx,cy,sx,sy);
	render_pixel_transform(cx,cy,sx,sy);
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

        // overlay
		if(1){
			glUseProgram(0);

			double overlaySizeX = 200,
				   overlaySizeY = 200;

			double xmin = (sx/2)-(overlaySizeX/2),
				   ymin = (sy/2)-(overlaySizeY/2),
				   xmax = (sx/2)+overlaySizeX/2,
				   ymax = (sy/2)+overlaySizeY/2;

			pixel_render_transform(&xmin,&ymin,sx,sy);
			pixel_render_transform(&xmax,&ymax,sx,sy);

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
