#version 460 core

layout (location = 0) in float height;

out vec4 out_color;

void main(void)
{
	out_color = vec4(vec3(height / 8.0f), 1.0f);
}