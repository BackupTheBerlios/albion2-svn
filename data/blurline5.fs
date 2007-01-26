// 1x5 kernel --- blur line 5

varying vec2 tex_coord;

uniform vec2 tcs[5];
uniform sampler2D texture;

void main(void)
{
	vec4 sample[5];
	
	for (int i = 0; i < 5; i++)
	{
		sample[i] = texture2D(texture, tex_coord + tcs[i]);
	}

//   7  26  41  26   7
//               / 107

	gl_FragColor = ((7.0*sample[0]) + (26.0*sample[1]) + (41.0*sample[2]) + (26.0*sample[3]) + (7.0*sample[4])) / 107.0;
}
