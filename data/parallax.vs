
uniform vec3 cam_pos;
uniform vec3 light_pos;
uniform mat4 local_mview;

attribute vec3 normal;
attribute vec3 binormal;
attribute vec3 tangent;
attribute vec2 texture_coord;

varying vec2 tex_coord;
varying vec3 light_vec;
varying vec3 view_vec;
varying float att;

void main() {
	gl_Position = ftransform();

	vec3 tvertex = vec3(local_mview * gl_Vertex);

	tex_coord = texture_coord;
	vec3 vlight = light_pos - tvertex;
	vec3 vview = cam_pos - tvertex;

	vec3 vv_tangent = vec3(local_mview * vec4(tangent, 1.0));
	vec3 vv_binormal = vec3(local_mview * vec4(binormal, 1.0));
	vec3 vv_normal = vec3(local_mview * vec4(normal, 1.0));

	light_vec.x = dot(vlight, vv_tangent);	// s tangent
	light_vec.y = dot(vlight, vv_binormal);	// t tangent
	light_vec.z = dot(vlight, vv_normal);	// normal

	view_vec.x = dot(vview, vv_tangent);	// s tangent
	view_vec.y = dot(vview, vv_binormal);	// t tangent
	view_vec.z = dot(vview, vv_normal);	// normal

	float d = length(vlight);
	att = 1.0 / (gl_LightSource[0].constantAttenuation + (gl_LightSource[0].linearAttenuation*d) + (gl_LightSource[0].quadraticAttenuation*d*d));
}
