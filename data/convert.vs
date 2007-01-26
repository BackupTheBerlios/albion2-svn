uniform vec2 half_pixel;

varying vec2 tex_coord0;
varying vec2 tex_coord1;
varying vec2 tex_coord2;
varying vec2 tex_coord3;

void main() {
	gl_Position = gl_Vertex;
	vec2 tex_coord = gl_Vertex.xy * 0.5 + 0.5;

	tex_coord0 = tex_coord + vec2(-1.0,  1.0) * half_pixel;
	tex_coord1 = tex_coord + vec2( 1.0,  1.0) * half_pixel;
	tex_coord2 = tex_coord + vec2( 1.0, -1.0) * half_pixel;
	tex_coord3 = tex_coord + vec2(-1.0, -1.0) * half_pixel;
}
