#version 330 core
out vec4 FragColour;

in vec2 TexCoords;

uniform sampler2D screenTexture0, 
				  screenTexture1, 
				  screenTexture2, 
				  screenTexture3, 
				  screenTexture4, 
				  screenTexture5, 
				  screenTexture6,
				  screenTexture7;

uniform mat4 view, proj;

void main(){
	FragColour = vec4(1 - vec3(texture(screenTexture0, TexCoords)), 1.0);
}