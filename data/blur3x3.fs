// 3x3 kernel

varying vec2 tex_coord;

uniform vec2 tcs[9];
uniform sampler2D texture;

void main(void)
{
	vec4 sample[9];
	
	for (int i = 0; i < 9; i++)
	{
		sample[i] = texture2D(texture, tex_coord + tcs[i]);
	}

//   1 2 1
//   2 1 2   / 13
//   1 2 1

	gl_FragColor = (sample[0]       + (2.0*sample[1]) + sample[2]       + 
			(2.0*sample[3]) + sample[4]       + (2.0*sample[5]) + 
			sample[6]       + (2.0*sample[7]) + sample[8]) / 13.0;
}
