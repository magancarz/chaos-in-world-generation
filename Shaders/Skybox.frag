#version 460 core

layout (location = 0) in vec3 sky_direction;

uniform vec3 sun_direction;

out vec4 out_color;

void main(void) {
	vec3 direction = normalize(sky_direction);
	float height = clamp(direction.y * 0.5 + 0.5, 0.0, 1.0);
	vec3 horizon_color = vec3(0.62, 0.76, 0.88);
	vec3 zenith_color = vec3(0.07, 0.24, 0.55);
	vec3 color = mix(horizon_color, zenith_color,
		smoothstep(0.35, 1.0, height));

	float sun_amount = max(dot(direction, normalize(sun_direction)), 0.0);
	color += vec3(1.0, 0.82, 0.52) * pow(sun_amount, 256.0);
	color += vec3(0.35, 0.24, 0.12) * pow(sun_amount, 16.0);

	out_color = vec4(color, 1.0);
}
