#version 460 core

layout (quads, fractional_odd_spacing, ccw) in;

uniform sampler2D height_map;
uniform mat4 projection_view;

layout (location = 0) in vec2 texture_coords[];

layout (location = 0) out float height;

void main(void)
{
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	vec2 t00 = texture_coords[0];
	vec2 t01 = texture_coords[1];
	vec2 t10 = texture_coords[2];
	vec2 t11 = texture_coords[3];

	vec2 t0 = (t01 - t00) * u + t00;
	vec2 t1 = (t11 - t10) * u + t10;
	vec2 tex_coord = (t1 - t0) * v + t0;

	height = texture(height_map, tex_coord).x * 8.0f;

	vec4 p00 = gl_in[0].gl_Position;
	vec4 p01 = gl_in[1].gl_Position;
	vec4 p10 = gl_in[2].gl_Position;
	vec4 p11 = gl_in[3].gl_Position;

	vec4 du = p01 - p00;
	vec4 dv = p10 - p00;
	vec4 normal = normalize(vec4(cross(dv.xyz, du.xyz), 0));

	vec4 p0 = (p01 - p00) * u + p00;
	vec4 p1 = (p11 - p10) * u + p10;
	vec4 p = (p1 - p0) * v + p0 + normal * height;

	gl_Position = projection_view * p;
}