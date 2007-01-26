uniform sampler2D scene_texture;
uniform sampler2D blur_texture;
uniform sampler2D exposure_texture;

varying vec2 tex_coord;

void main() {
	float exposure = texture2D(exposure_texture, tex_coord).x;
	vec3 scene = texture2D(scene_texture, tex_coord).rgb;
	vec3 blur  = texture2D(blur_texture, tex_coord).rgb;

	// expand fixed point HDR results to its full range
	blur *= 128.0;

	// compose and reduce to low dynamic range for output
	vec3 color = scene + blur;
	color = 1.0 - exp2(-exposure * color.rgb);

	gl_FragColor.rgb = color;
}
