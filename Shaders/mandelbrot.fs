#version 330 core

uniform int iterations;

uniform float time;
uniform vec2 user_pos;
uniform float user_scale;

uniform vec2 screenSize;

vec2 product(vec2 a, vec2 b) {
	return vec2(a.x*b.x-a.y*b.y, a.x*b.y+a.y*b.x);
}

int mandelbrot(vec2 c) {
	float last_magnitude = 0;
	int converge_count = 0;
	vec2 z;
	for (int i = 0; i < iterations; i++) {
		z = product(z,z) + c;
		float mag = length(z);
		if (mag <= last_magnitude)
			converge_count++;
		last_magnitude = mag;
	}

	return converge_count;
}

void main() {
	vec2 uv = gl_FragCoord.xy / screenSize;
	//https://gamedev.stackexchange.com/questions/83853/how-to-implement-color-changing-fragment-shader
	uv = 2.0 * uv - 1.0;
	uv *= pow(1.1, -user_scale);
	uv += user_pos;
	

	int cc = mandelbrot(uv);
	if (float(cc)/iterations > 0.1) {
		gl_FragColor = vec4(1);
	} else {
		discard;
	}
//	gl_FragColor = vec4(float(cc)/iterations);
}