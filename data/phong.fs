// thx to http://www.ozone3d.net/ for that phong lighting tutorial

varying vec3 normal, lightDir, eyeVec;
varying float att;
varying vec2 tex_coord;

uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;

void main (void)
{
	vec4 diffuse_color = texture2D(diffuse_texture, tex_coord);
	vec4 specular_color = texture2D(specular_texture, tex_coord);

	vec4 final_color = (gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient) + (gl_LightSource[0].ambient * gl_FrontMaterial.ambient)*att;
							
	vec3 N = normalize(normal);
	vec3 L = normalize(lightDir);
	
	float lambertTerm = dot(N,L);
	
	if(lambertTerm > 0.0)
	{
		final_color += gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * lambertTerm * att;	
		
		vec3 E = normalize(eyeVec);
		vec3 R = reflect(-L, N);
		
		float specular = pow( max(dot(R, E), 0.0), gl_FrontMaterial.shininess );
		
		final_color += gl_LightSource[0].specular * gl_FrontMaterial.specular * specular * specular_color * att;
	}

	gl_FragColor = diffuse_color * final_color;
}
