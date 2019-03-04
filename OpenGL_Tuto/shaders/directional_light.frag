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
    
	float bias = max(0.05 * (1.0 - dot(Normal, lightDir)), 0.005); 
	vec4 proj4 = lightMatrix * vec4(FragPos, 1.0);
	vec3 proj = proj4.xyz/proj4.w;
	vec2 projCoords = proj.xy / 2 + 0.5;
	float currentDepth = proj.z;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	float shadow = 0;
	for(int x = -2; x <= 2; ++x)
	{
		for(int y = -2; y <= 2; ++y)
		{
			float closestDepth = texture(shadowMap, projCoords + vec2(x, y) * texelSize).x;
			shadow += (currentDepth + 1) / 2 - bias > closestDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 16.0;
	//float shadow = currentDepth < 1.0 && (currentDepth + 1) / 2 - bias > closestDepth ? 1 : 0;
	float displayValue = shadow;
    FragColor = vec4(lighting * (1 - shadow), 1.0);
}  