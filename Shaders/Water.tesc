#version 460 core

layout (vertices = 4) out;

layout (location = 0) in vec2 pass_texture_coords[];
layout (location = 0) out vec2 texture_coords[];

void main(void) {
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	texture_coords[gl_InvocationID] = pass_texture_coords[gl_InvocationID];

	if (gl_InvocationID == 0) {
		gl_TessLevelOuter[0] = 1.0;
		gl_TessLevelOuter[1] = 1.0;
		gl_TessLevelOuter[2] = 1.0;
		gl_TessLevelOuter[3] = 1.0;
		gl_TessLevelInner[0] = 1.0;
		gl_TessLevelInner[1] = 1.0;
	}
}
