#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
    vec3 Direction;
    vec3 Color;
	float Intensity;
};

uniform Light light;
uniform vec3 viewPos;

void main()
{             
    // retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    
    // then calculate lighting as usual
    vec3 lighting = vec3(0, 0, 0);
    vec3 viewDir = normalize(viewPos - FragPos);
    // diffuse
    vec3 lightDir = light.Direction;
	vec3 norm = normalize(Normal);
	
    vec3 diffuse = max(dot(norm, lightDir), 0.0) * light.Color;
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0f) * max(dot(norm, lightDir), 0) * Specular;
    lighting += spec + diffuse;
    
    FragColor = vec4(lighting, 1.0);
}  