#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aSegmentAPos;
layout (location = 2) in vec3 aSegmentBPos;

out vec2 AFragPos;
out vec2 BFragPos;
out vec3 FragPos;

uniform mat4 projection, view, model;

void main()
{
	mat4 transform = projection * view * model;
    vec4 hFragPos = transform * vec4(aPos, 1.0);
	FragPos = hFragPos.xyz / hFragPos.w;
	vec4 hAFragPos = transform * vec4(aSegmentAPos, 1.0);
	AFragPos = hAFragPos.xyz / hAFragPos.w;
	vec4 hBFragPos = transform * vec4(aSegmentBPos, 1.0);
	BFragPos = hBFragPos.xyz / hBFragPos.w;
}  