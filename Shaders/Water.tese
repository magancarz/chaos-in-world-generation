#version 460 core

layout (quads, equal_spacing, ccw) in;

uniform mat4 projection_view;
uniform float water_height;

layout (location = 0) in vec2 texture_coords[];

void main(void) {
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	vec4 position0 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, u);
	vec4 position1 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, u);
	vec4 position = mix(position0, position1, v);
	position.y = water_height;
	gl_Position = projection_view * position;
}
