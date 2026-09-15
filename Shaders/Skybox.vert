#version 460 core

layout (location = 0) in vec3 position;

layout (location = 0) out vec3 pass_direction;

void main(void) {
	gl_Position = vec4(position, 1.0);
	pass_direction = position;
}
