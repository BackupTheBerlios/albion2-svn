// 5x5 kernel

varying vec2 tex_coord;

uniform vec2 tcs[25];
uniform sampler2D texture;

void main(void)
{
	vec4 sample[25];
	
	for (int i = 0; i < 25; i++)
	{
		sample[i] = texture2D(texture, tex_coord + tcs[i]);
	}

//   1   4   7   4   1
//   4  16  26  16   4
//   7  26  41  26   7
//   4  16  26  16   4
//   1   4   7   4   1
//               / 273

	gl_FragColor = (sample[0]        + (4.0*sample[1])   + (7.0*sample[2])   + (4.0*sample[3])   + sample[4]        +
			(4.0*sample[5])  + (16.0*sample[6])  + (26.0*sample[7])  + (16.0*sample[8])  + (4.0*sample[9])  +
			(7.0*sample[10]) + (26.0*sample[11]) + (41.0*sample[12]) + (26.0*sample[13]) + (7.0*sample[14]) +
			(4.0*sample[15]) + (16.0*sample[16]) + (26.0*sample[17]) + (16.0*sample[18]) + (4.0*sample[19]) +
			sample[20]       + (4.0*sample[21])  + (7.0*sample[22])  + (4.0*sample[23])  + sample[24]) / 273.0;
}
