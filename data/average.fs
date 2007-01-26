uniform sampler2D RT;

varying vec2 tex_coord;

void main() {
	gl_FragColor = texture2D(RT, tex_coord);
}
