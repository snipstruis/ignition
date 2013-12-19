uniform vec2  screen;
uniform vec2  center;
uniform int mode;
uniform float scale;
uniform float startx;
uniform float starty;

float smooth(float x, float y, float i){
	float zn = sqrt( x*x + y*y );
	float nu = log( log(zn) / log(2) ) / log(2);
	float iteration = float(i) + 1.0 - nu;
	return iteration;
}

float mandelbrot(vec2 p){
	float x=p.x;
	float y=p.y;
	const int maxiter = 256;
	for(int i=0; i<maxiter; i++){
		float xtemp = x*x - y*y + p.x;
		y = 2.0*x*y + p.y;
		x = xtemp;
		if(x*x + y*y > 4.0) return smooth(x, y, float(i))/float(maxiter);
	}
	return 0.0;
}
float juliaset(vec2 p){
	if(mode == 0)p *= 1.3;

	float x=-p.x;
	float y=p.y;

	const int maxiter = 256 ;
	for(int i=0; i<maxiter; i++){
		float xtemp = x*x - y*y + startx;
		y = 2.0*x*y + starty;
		x = xtemp;
		if(x*x + y*y > 4.0) return smooth(x, y, float(i))/float(maxiter);
	}
	return 1.0;
}

vec4 color(float mandel, float julia){
	return vec4(julia*2.0, julia/2.0+mandel/2.0, mandel*2.0, 1.0);
}

void main() {
	// position square [(0,0),(1,1)]
	vec2 pixel = gl_FragCoord.xy/min(screen.x,screen.y);

	// center
	if(screen.x>screen.y){
		pixel.x -= (screen.x/screen.y - 1.0)/2.0;
	}else{
		pixel.y -= (screen.y/screen.x - 1.0)/2.0;
	}

	pixel = 2.0 * pixel - 1.0;

	// clear pixel
	gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);

	// window space
	if(mode==0) gl_FragColor += color(0.0, juliaset(pixel));

	// to object space
	pixel /= scale;
	pixel += center;

	// object space
	if(mode==0) gl_FragColor += color(mandelbrot(pixel),0.0);
	if(mode==1) gl_FragColor += color(0.0, juliaset(pixel));
}
