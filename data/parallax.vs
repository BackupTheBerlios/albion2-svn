
uniform vec3 cam_pos;
uniform vec3 light_pos;

attribute vec3 normal;
attribute vec3 binormal;
attribute vec3 tangent;
attribute vec2 texture_coord;

varying vec2 tex_coord;
varying vec3 light_vec;
varying vec3 view_vec;

void main() {
	gl_Position = ftransform();

	tex_coord = texture_coord;
	vec3 vlight = light_pos - gl_Vertex.xyz;
	vec3 vview = cam_pos - gl_Vertex.xyz;

	// Transform vectors into tangent space
	light_vec.x = dot(vlight, tangent);
	light_vec.y = dot(vlight, binormal);
	light_vec.z = dot(vlight, normal);

	view_vec.x = dot(vview, tangent);
	view_vec.y = dot(vview, binormal);
	view_vec.z = dot(vview, normal);
}
