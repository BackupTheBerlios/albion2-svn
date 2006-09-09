
attribute vec2 texture_coord;
varying vec2 tex_coord;

void main()
{
	gl_Position = gl_Vertex;
	tex_coord = texture_coord;
}
