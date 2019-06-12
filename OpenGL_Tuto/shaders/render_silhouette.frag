#version 330 core

layout (location = 0) out vec2 gOffset;

uniform vec2 screenSize;
in vec3 FragPos;
in vec3 APos;
in vec3 BPos;

void main()
{
	vec2 screenFragPos = round(screenSize * FragPos);
	vec2 samplePoint = screenSize * APos;
	vec2 offset = samplePoint - screenFragPos;

}