#pragma once

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
	fixed_aspectratio(&arx,&ary);
	//*cy = -*cy;
	*cx = ((*cx * arx) / uni_scale) + uni_center_x;
	*cy = ((*cy * ary) / uni_scale) + uni_center_y;
}

void world_render_transform(double* cx, double* cy,int sx,int sy){
	double arx = (double)sx, ary = (double)sy;
	fixed_aspectratio(&arx,&ary);
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
