#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D shadowMap;
uniform mat4 lightMatrix;

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
    
	vec4 proj4 = lightMatrix * vec4(FragPos, 1.0);
	vec3 proj = proj4.xyz/proj4.w;
	float closestDepth = texture(shadowMap, proj.xy / 2 + 0.5).x;
	float currentDepth = proj.z;
	float shadow = currentDepth - 0.005 > closestDepth ? 1: 0;
	float displayValue =  - currentDepth + closestDepth;
    FragColor = vec4 (displayValue, displayValue, displayValue, 1);
}  