// thx to http://www.ozone3d.net/ for that phong lighting tutorial

varying vec3 normal, lightDir, eyeVec;
varying float att;
varying vec2 tex_coord;

void main()
{	
	normal = gl_NormalMatrix * gl_Normal;

	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	lightDir = vec3(gl_LightSource[0].position.xyz - vVertex);
	eyeVec = -vVertex;
	
	float d = length(lightDir);
	
	att = 1.0 / ( gl_LightSource[0].constantAttenuation + 
	(gl_LightSource[0].linearAttenuation*d) + 
	(gl_LightSource[0].quadraticAttenuation*d*d) );

	tex_coord = gl_MultiTexCoord0.xy;

	gl_Position = ftransform();
}
