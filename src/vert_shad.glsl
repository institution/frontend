#version 120

attribute float mass;
attribute vec3 pos;

uniform mat4 mvp;
uniform mat4 m_transform;

varying float f_mass;

void main(void) {
	gl_Position = mvp * m_transform * vec4(pos, 1.0);
	f_mass = mass;
}