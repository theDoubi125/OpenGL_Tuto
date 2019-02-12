#version 330 core
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;

struct Material
{
	vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light
{
	int count;
	vec3 ambient;

	vec3 positions[10];
	vec3 diffuse[10];
	vec3 specular[10];
};

uniform Material material;
uniform Light light;

void main()
{
	vec3 norm = normalize(Normal);

    vec3 ambient = light.ambient * material.ambient;
	
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 lightDiffuse = vec3(0);
	vec3 lightSpecular = vec3(0);
	for(int i=0; i<light.count; i++)
	{
		vec3 lightDir = normalize(light.positions[i] - FragPos);
		float diff = max(dot(lightDir, norm), 0);
		lightDiffuse += light.diffuse[i] * diff;
		
		vec3 reflectDir = reflect(-lightDir, norm);
		
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * max(dot(norm, lightDir), 0);
		lightSpecular += light.specular[i] * spec;
	}

	vec3 specular = lightSpecular * vec3(texture(material.specular, TexCoords));

	vec3 diffuse = lightDiffuse * vec3(texture(material.diffuse, TexCoords));


    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}

