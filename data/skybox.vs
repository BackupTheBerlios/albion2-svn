varying vec3 cube_coord;

void main() {
	gl_Position = gl_Vertex;
	cube_coord = gl_MultiTexCoord0.xyz;
}
