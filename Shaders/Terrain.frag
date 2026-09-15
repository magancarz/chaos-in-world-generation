#version 460 core

layout (location = 0) in vec3 terrain_color;
layout (location = 1) in vec3 terrain_normal;

uniform vec3 sun_position;

out vec4 out_color;

void main(void) {
	vec3 color = clamp(dot(sun_position, terrain_normal), 0.0, 1.0)
                   * terrain_color;
	out_color = vec4(color, 1.0);
}
