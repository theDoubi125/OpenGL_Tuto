#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 color;

out vec4 VertexColor;
uniform mat4 projection, view;

void main()
{
    gl_Position = projection * view * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	VertexColor = color;
}