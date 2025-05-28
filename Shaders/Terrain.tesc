#version 460 core

layout (vertices = 4) out;

uniform mat4 view;

layout (location = 0) in vec2 pass_texture_coords[];

layout (location = 0) out vec2 texture_coords[];

void main(void)
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	texture_coords[gl_InvocationID] = pass_texture_coords[gl_InvocationID];

	if (gl_InvocationID == 0)
	{
		const int MIN_TESS_LEVEL = 4;
		const int MAX_TESS_LEVEL = 64;
		const float MIN_DISTANCE = 20;
		const float MAX_DISTANCE = 800;

		vec4 eye_space_pos00 = view * gl_in[0].gl_Position;
		vec4 eye_space_pos01 = view * gl_in[1].gl_Position;
		vec4 eye_space_pos10 = view * gl_in[2].gl_Position;
		vec4 eye_space_pos11 = view * gl_in[3].gl_Position;

		float distance00 = clamp((abs(eye_space_pos00.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
		float distance01 = clamp((abs(eye_space_pos01.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
		float distance10 = clamp((abs(eye_space_pos10.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
		float distance11 = clamp((abs(eye_space_pos11.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);

		float tess_level0 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance10, distance00));
		float tess_level1 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance00, distance01));
		float tess_level2 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance01, distance11));
		float tess_level3 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance11, distance10));

		gl_TessLevelOuter[0] = tess_level0;
		gl_TessLevelOuter[1] = tess_level1;
		gl_TessLevelOuter[2] = tess_level2;
		gl_TessLevelOuter[3] = tess_level3;

		gl_TessLevelInner[0] = max(tess_level1, tess_level3);
		gl_TessLevelInner[1] = max(tess_level0, tess_level2);
	}
}