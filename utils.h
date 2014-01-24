#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void screenshot(GLFWwindow* window, char* filename){
	// prevents padding
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	// get pixeldata
	int sx,sy; glfwGetFramebufferSize(window,&sx,&sy);
	uint8_t* pixeldata = malloc(sx*sy*3);
	if(!pixeldata){ fprintf(stderr,"cant allocate memory for screenshot"); return;}
	glReadPixels(0, 0, sx, sy, GL_BGR, GL_UNSIGNED_BYTE, pixeldata);

	// get TGA header
	uint8_t header[18]={
		0,0,2,0,0,0,0,0,0,0,0,0,
		sx%256,sx/256, sy%256,sy/256, 24,0
	};

	// write to file
	FILE *f = fopen(filename,"wb");
		fwrite(header,    1, 18,      f);
		fwrite(pixeldata, 1, sx*sy*3, f);
	fclose(f);

	//cleanup!
	free(pixeldata);
}

char* read_file(const char* filename){
	FILE* fp;
	fp = fopen(filename,"r");
	if(!fp) fprintf(stderr,"error while reading file %s\n",filename);

	fseek(fp,0,SEEK_END);
	size_t len = ftell(fp);
	rewind(fp);

	char* r = (char*) malloc(sizeof(char) * len + 1);
	if(!r)fprintf(stderr,"can't allocate the memory to hold %s\n",filename);
	fread(r,sizeof(char),len,fp);
	r[len] = '\0';

	fscanf(fp,"%s",r);
	fclose(fp);

	return r;
}

// initialising shaders
GLint use_shader(const char* frag_src){
	// create empty shader-objects
	GLuint frag_o = glCreateShader(GL_FRAGMENT_SHADER);

	// fill shader with sourcecode
	const int frag_len = strlen(frag_src);
	glShaderSource(frag_o, 1, &frag_src, &frag_len);

	// compile said sourcecode
	glCompileShader(frag_o);

	// check for errors
	GLint compiled;
	glGetShaderiv(frag_o, GL_COMPILE_STATUS, &compiled);
	if(!compiled){
		GLint log_len;
		glGetShaderiv(frag_o, GL_INFO_LOG_LENGTH, &log_len);

		//The maxLength includes the NULL character
		GLchar* log_txt = malloc(sizeof(GLchar) * log_len + 1);
		glGetShaderInfoLog(frag_o, log_len, &log_len, &log_txt[0]);
		fprintf(stderr,"GLSL: compile error:\n%s\n",log_txt);
		exit(EXIT_FAILURE);
	} else printf("GLSL: shaders successfully compiled\n");

	// create empty program
	GLuint prog = glCreateProgram();

	// attatch shader to the program
	glAttachShader(prog,frag_o);

	// link the program
	glLinkProgram(prog);

	// use program
	glUseProgram(prog);

	return prog;
}

void update_julia(float cx, float cy, float sx, float sy){
	sx /= 2.0;
	sy /= 2.0;
	uni_startx = ((cx-sx)/(uni_scale*fmin(sx,sy)))+uni_center_x;
	uni_starty = ((cy-sy)/(uni_scale*fmin(sx,sy)))-uni_center_y;
	sync_startx();
	sync_starty();
}

void edgescroll(double cx, double cy, int sx, int sy){
	const float scrollspeed = 0.025;
	const float border = 10.0;
	if		(cx > (double)sx-border) uni_center_x += scrollspeed/uni_scale;
	else if	(cx < border)			 uni_center_x -= scrollspeed/uni_scale;
	if		(cy > (double)sy-border) uni_center_y -= scrollspeed/uni_scale;
	else if	(cy < border)			 uni_center_y += scrollspeed/uni_scale;
	sync_center();
}

void fixed_aspectratio(double* arx, double* ary){
	double x = *arx, y = *ary;
	*arx /= fmin(x, y);
	*ary /= fmin(x, y);
}
