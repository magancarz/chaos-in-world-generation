#version 460 core

layout (quads, fractional_odd_spacing, ccw) in;

uniform sampler2D height_map;
uniform mat4 projection_view;

layout (location = 0) in vec2 texture_coords[];

layout (location = 0) out vec3 terrain_color;
layout (location = 1) out vec3 terrain_normal;

void main(void) {
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	vec2 t00 = texture_coords[0];
	vec2 t01 = texture_coords[1];
	vec2 t10 = texture_coords[2];
	vec2 t11 = texture_coords[3];

	vec2 t0 = (t01 - t00) * u + t00;
	vec2 t1 = (t11 - t10) * u + t10;
	vec2 tex_coord = (t1 - t0) * v + t0;

	vec4 terrain_sample = texture(height_map, tex_coord);
	float height = terrain_sample.a;
	terrain_color = terrain_sample.rgb;

	vec4 p00 = gl_in[0].gl_Position;
	vec4 p01 = gl_in[1].gl_Position;
	vec4 p10 = gl_in[2].gl_Position;
	vec4 p11 = gl_in[3].gl_Position;

	vec4 p0 = (p01 - p00) * u + p00;
	vec4 p1 = (p11 - p10) * u + p10;

	vec3 position_du = mix(p01.xyz - p00.xyz, p11.xyz - p10.xyz, v);
	vec3 position_dv = p1.xyz - p0.xyz;
	vec3 base_normal = normalize(cross(position_dv, position_du));

	vec2 texel_size = 1.0 / vec2(textureSize(height_map, 0));
	vec2 left_coord = vec2(max(tex_coord.x - texel_size.x, 0.0), tex_coord.y);
	vec2 right_coord = vec2(min(tex_coord.x + texel_size.x, 1.0), tex_coord.y);
	vec2 down_coord = vec2(tex_coord.x, max(tex_coord.y - texel_size.y, 0.0));
	vec2 up_coord = vec2(tex_coord.x, min(tex_coord.y + texel_size.y, 1.0));

	float left_height = textureLod(height_map, left_coord, 0.0).a;
	float right_height = textureLod(height_map, right_coord, 0.0).a;
	float down_height = textureLod(height_map, down_coord, 0.0).a;
	float up_height = textureLod(height_map, up_coord, 0.0).a;
	vec2 height_gradient = vec2(
		(right_height - left_height) / max(right_coord.x - left_coord.x, 1e-6),
		(up_height - down_height) / max(up_coord.y - down_coord.y, 1e-6));

	vec2 texture_du = mix(t01 - t00, t11 - t10, v);
	vec2 texture_dv = t1 - t0;
	vec3 displaced_du = position_du + base_normal * dot(height_gradient, texture_du);
	vec3 displaced_dv = position_dv + base_normal * dot(height_gradient, texture_dv);
	terrain_normal = normalize(cross(displaced_dv, displaced_du));

	vec4 p = (p1 - p0) * v + p0 + vec4(base_normal, 0.0) * height;

	gl_Position = projection_view * p;
}
