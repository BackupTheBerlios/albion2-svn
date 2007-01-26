uniform sampler2D average_texture;
uniform sampler2D exposure_texture;

uniform float frame_time;

varying vec2 tex_coord;

void main() {
	// sample 1x1 px mipmap level
	vec3 average = texture2D(average_texture, tex_coord, 10.0).rgb;
	float old_exp = texture2D(exposure_texture, tex_coord).x;

	// compute new exposure value
	float new_exp = 1.4427 / (0.5 + dot(average, vec3(128.0)));

	gl_FragColor.x = mix(old_exp, new_exp, frame_time);
}
