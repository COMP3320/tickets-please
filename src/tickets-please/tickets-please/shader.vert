#version 150

in vec3 position;
in vec3 colour;
in vec2 texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 Colour;
out vec2 Texcoord;

void main()
{
	Texcoord = texcoord;
	Colour = colour;
    gl_Position = proj * view * model * vec4(position, 1.0);
}

