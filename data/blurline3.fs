// 1x3 kernel --- blur line 3

varying vec2 tex_coord;

uniform vec2 tcs[3];
uniform sampler2D texture;

void main(void)
{
	vec4 sample[3];
	
	for (int i = 0; i < 3; i++)
	{
		sample[i] = texture2D(texture, tex_coord + tcs[i]);
	}

//   2 1 2
//     / 5

	gl_FragColor = ((2.0*sample[0]) + sample[1] + (2.0*sample[2])) / 5.0;
}
