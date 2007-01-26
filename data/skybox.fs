uniform samplerCube skybox_texture;
uniform float max_value;

varying vec3 cube_coord;

void main() {
	vec4 sky = textureCube(skybox_texture, cube_coord);

	// expand to floating point
	gl_FragColor.rgb = sky.rgb * sky.a * max_value;
}
