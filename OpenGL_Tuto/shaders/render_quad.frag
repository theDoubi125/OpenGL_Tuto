#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gLight;

uniform vec3 viewPos;

void main()
{             
    // retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
	vec3 Light = texture(gLight, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    
    FragColor = vec4(Albedo * (Light + 0.1), 1.0);
}  