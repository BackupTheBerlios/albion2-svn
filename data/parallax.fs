
uniform sampler2D diffuse_map;
uniform sampler2D normal_map;
uniform sampler2D height_map;

uniform vec4 light_color;

varying vec2 tex_coord;
varying vec3 light_vec;
varying vec3 view_vec;

void main() {
	vec3 vview = normalize(view_vec);
	vec3 vlight = normalize(light_vec);

	float parallax = 0.03;
	
	float height = texture2D(height_map, tex_coord).x;
	float offset = parallax * (2.0 * height - 1.0);
	vec2 parallax_tex_coord = tex_coord + offset * vview.xy;

	height += texture2D(height_map, parallax_tex_coord).x;
	offset = parallax * (height - 1.0);
	parallax_tex_coord = tex_coord + offset * vview.xy;

	vec4 base = texture2D(diffuse_map, parallax_tex_coord);
	vec3 bump = normalize(texture2D(normal_map, parallax_tex_coord).xyz * 2.0 - 1.0);

	float diffuse = clamp(dot(vlight, bump), 0.0, 1.0);
	float specular = pow(clamp(dot(reflect(-vview, bump), vlight), 0.0, 1.0), 12.0);

	vec4 lighting = (diffuse * base + 0.5 * specular);

	gl_FragColor = lighting + light_color * base;
}
