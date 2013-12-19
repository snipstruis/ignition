#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

static void error_callback(int error, const char* description){
	fprintf(stderr,"glfw_error: %s\n",description);
}

static void key_callback(GLFWwindow* window, int key,
							  int scancode, int action, int mods){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X  && action == GLFW_PRESS){
		uni_scale = 1.0;
		if(uni_mode == 0)uni_center_x = -0.5;
		else uni_center_x = 0.0;
		uni_center_y = 0.0;
		sync_center();
		sync_scale();
	}
	if (key == GLFW_KEY_S  && action == GLFW_PRESS){
		if(uni_mode == 0) {
			uni_mode = 1;
			uni_scale = 1.0/1.3;
			uni_center_x = 0.0;
		} else {
			uni_mode = 0;
			uni_scale = 1.0;
			uni_center_x = -0.5;
		}
		uni_center_y = 0.0;
		sync_center();
		sync_scale();

		sync_mode();
	}
	if (key == GLFW_KEY_Q  && action == GLFW_PRESS){
		char filename[50];
		sprintf(filename,"screenshot_%d.tga",time());
		printf("saving screenshot to \"%s\" ...",filename);
		screenshot(window,filename);
		printf("done!\n");
	}
}

static void framebuffer_size_callback(GLFWwindow* window, int width,int height){
	glViewport(0, 0, width, height);
	glUniform2f(loc_screen,(GLfloat)width,(GLfloat)height);
}

static void scroll_callback(GLFWwindow* window,double x, double y){
	uni_scale *= 1.0 + y*0.1;
	sync_scale();
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
	if(action == GLFW_PRESS){
		double cx,cy;
		glfwGetCursorPos(window,&cx,&cy);
		int sx,sy;
		glfwGetFramebufferSize(window,&sx,&sy);

		sx /= 2.0;
		sy /= 2.0;
		uni_center_x += (cx-sx)/(uni_scale*fmin(sx,sy));
		uni_center_y -= (cy-sy)/(uni_scale*fmin(sx,sy));

		sync_center();
	}
}

#pragma clang diagnostic pop

void init_callbacks(GLFWwindow* window){
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
	glfwSetMouseButtonCallback(window,mouse_button_callback);
	glfwSetScrollCallback(window,scroll_callback);
}
