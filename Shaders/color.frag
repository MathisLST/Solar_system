#version 130
precision mediump float; //Medium precision for float. highp and smallp can also be used


varying vec3 Normal;
varying vec3 FragPosition;
varying vec2 vary_uv;

uniform vec3 viewPosition;

struct Planet {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
	int two;
};
uniform Planet planet;

struct Light {
	vec3 position;
	vec3 ambiant;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light;

void main()
{
	// AMBIANT
	vec3 ambiant = light.ambiant * vec3(texture(planet.diffuse, vary_uv));

	// DIFFUSE
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPosition);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(planet.diffuse, vary_uv));

	// SPECULAR
	vec3 viewDir = normalize(viewPosition - FragPosition);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(reflectDir, viewDir), 0.0), planet.shininess);
	vec3 specular = light.specular * spec;
	// si on a deux textures
	if (planet.two == 1)
	{ specular *= vec3(texture(planet.specular, vary_uv)); }

	vec3 result = ambiant + diffuse + specular; 
	gl_FragColor = vec4(result, 1.0);
}