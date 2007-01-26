uniform sampler2D RT;

varying vec2 tex_coord0;
varying vec2 tex_coord1;
varying vec2 tex_coord2;
varying vec2 tex_coord3;

void main() {
	vec4 rt = texture2D(RT, tex_coord0);
	rt += texture2D(RT, tex_coord1);
	rt += texture2D(RT, tex_coord2);
	rt += texture2D(RT, tex_coord3);

	rt.rgb /= (4.0 * 128.0);

	gl_FragColor = rt;
}
