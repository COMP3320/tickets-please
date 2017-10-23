#version 330 core

uniform int code;

out vec4 FragColor;

void main()
{
	FragColor = vec4(code/255.0);
	//outputF = vec4(1,0,0,1);
} 
