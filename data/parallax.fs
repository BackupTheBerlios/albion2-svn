
uniform sampler2D diffuse_texture;
uniform sampler2D normal_texture;
uniform sampler2D height_texture;
uniform sampler2D specular_texture;

varying vec2 tex_coord;
varying vec3 light_vec;
varying vec3 view_vec;
varying float att;

void main() {
	vec3 vview = normalize(view_vec);
	vec3 vlight = normalize(light_vec);

	// determines the "deepness"
	float parallax = 0.03;

	// compute parallax tex coord
	float height = texture2D(height_texture, tex_coord).x;
	float offset = parallax * (2.0 * height - 1.0);
	vec2 parallax_tex_coord = tex_coord + offset * vview.xy;

	// refinement #1
	height += texture2D(height_texture, parallax_tex_coord).x;
	offset = parallax * (height - 1.0);
	parallax_tex_coord = tex_coord + offset * vview.xy;

	// refinement #2
	height += texture2D(height_texture, parallax_tex_coord).x;
	offset = parallax * (2.0/3.0 * height - 1.0);
	parallax_tex_coord = tex_coord + offset * vview.xy;

	// lookup diffuse, normal and specular texture color (with transformed (parallax) tex coord)
	vec4 base = texture2D(diffuse_texture, parallax_tex_coord);
	vec3 bump = normalize(texture2D(normal_texture, parallax_tex_coord).xyz * 2.0 - 1.0);
	vec4 spec = texture2D(specular_texture, parallax_tex_coord);

	// compute light terms
	float lambertTerm = clamp(dot(vlight, bump), 0.0, 1.0);

	vec4 final_color = (gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient) + (gl_LightSource[0].ambient * gl_FrontMaterial.ambient) * att;
	if(lambertTerm > 0.0) {
		final_color += gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * lambertTerm * att;

		vec3 R = reflect(-vview, bump);
		float specular = pow( max(dot(R, vlight), 0.0), gl_FrontMaterial.shininess );

		final_color += gl_LightSource[0].specular * gl_FrontMaterial.specular * specular * att * spec;
	}
	gl_FragColor = base * final_color;
}
