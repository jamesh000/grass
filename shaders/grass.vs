#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 3) in mat4 aInstanceMatrix;

out vec2 TexCoord;
out vec4 FragCoord;

uniform mat4 p;
uniform mat4 v;

void main()
{
        FragCoord = p * v * aInstanceMatrix * vec4(aPos, 1.0);
	gl_Position = FragCoord;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}