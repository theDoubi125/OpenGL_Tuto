#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 projection, view, model;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
	vec4 worldPos = model * vec4(aPos, 1.0);
    gl_Position = projection * view * worldPos;
	Normal = mat3(transpose(inverse(model))) * aNormal;  
	FragPos = worldPos.xyz / worldPos.w;
	TexCoords = aTexCoords;
}