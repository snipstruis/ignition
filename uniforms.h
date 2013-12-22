#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// uniform locations
static GLint loc_screen;
static GLint loc_scale;
static GLint loc_center;
static GLint loc_startx;
static GLint loc_starty;
static GLint loc_mode;

// uniform values, change them and then call the update function.
GLfloat uni_center_x = 0.0;//-0.5;
GLfloat uni_center_y = 0.0;
GLfloat uni_scale    = 1.0;

GLfloat uni_startx = 0.0;
GLfloat uni_starty = 0.0;
GLint uni_mode = 0;

// update functions
void sync_center(){
	glUniform2f(loc_center,uni_center_x,uni_center_y);
}

void sync_scale(){
	glUniform1f(loc_scale,uni_scale);
}

void sync_startx(){
	glUniform1f(loc_startx, uni_startx);
}
void sync_starty(){
	glUniform1f(loc_starty, uni_starty);
}
void sync_mode(){
	glUniform1i(loc_mode, uni_mode);
}

// initialize
void init_uniforms(GLint prog){
	// bind uniforms
	loc_center = glGetUniformLocation(prog,"center");
	loc_scale  = glGetUniformLocation(prog,"scale");
	loc_screen = glGetUniformLocation(prog,"screen");

	loc_startx = glGetUniformLocation(prog,"startx");
	loc_starty = glGetUniformLocation(prog,"starty");
	loc_mode   = glGetUniformLocation(prog,"mode");

	// set uniform defaults
	sync_scale();
	sync_center();
}

