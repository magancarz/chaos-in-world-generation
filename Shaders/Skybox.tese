#version 460 core

layout (quads, equal_spacing, ccw) in;

uniform mat4 projection_view_rotation;

layout (location = 0) in vec3 direction[];
layout (location = 0) out vec3 sky_direction;

void main(void) {
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	vec3 direction0 = mix(direction[0], direction[1], u);
	vec3 direction1 = mix(direction[2], direction[3], u);
	sky_direction = mix(direction0, direction1, v);

	vec4 clip_position = projection_view_rotation * vec4(sky_direction, 1.0);
	gl_Position = clip_position.xyww;
}
