#version 120


attribute vec3 point;
attribute vec3 color;

uniform mat4 mvp;
uniform mat4 transform;

varying vec3 f_color;

void main(void) {
	gl_Position = mvp * transform * vec4(point, 1.0);
	f_color = color;
}