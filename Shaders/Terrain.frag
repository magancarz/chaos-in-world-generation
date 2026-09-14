#version 460 core

layout (location = 0) in vec3 terrain_color;
layout (location = 1) in float normalized_height;

uniform int visualization_mode;

out vec4 out_color;

void main(void)
{
	vec3 color = visualization_mode == 1 ? vec3(normalized_height) : terrain_color;
	out_color = vec4(color, 1.0f);
}
