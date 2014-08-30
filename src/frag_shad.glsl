#version 120

varying float f_mass;

void main(void) {
	
	gl_FragColor = vec4(f_mass, 1, 1, 1);

}