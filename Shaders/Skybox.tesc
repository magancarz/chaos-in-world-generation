#version 460 core

layout (vertices = 4) out;

layout (location = 0) in vec3 pass_direction[];
layout (location = 0) out vec3 direction[];

void main(void) {
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	direction[gl_InvocationID] = pass_direction[gl_InvocationID];

	if (gl_InvocationID == 0) {
		gl_TessLevelOuter[0] = 1.0;
		gl_TessLevelOuter[1] = 1.0;
		gl_TessLevelOuter[2] = 1.0;
		gl_TessLevelOuter[3] = 1.0;
		gl_TessLevelInner[0] = 1.0;
		gl_TessLevelInner[1] = 1.0;
	}
}
