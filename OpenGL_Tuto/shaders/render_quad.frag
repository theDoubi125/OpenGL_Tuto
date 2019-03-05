#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D finalTexture;

uniform vec3 viewPos;

void main()
{
    FragColor = texture(finalTexture, TexCoords);
}  