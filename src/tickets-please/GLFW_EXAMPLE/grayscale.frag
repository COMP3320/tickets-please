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
	FragColour = texture(screenTexture0, TexCoords);
	float average = 0.2126 * FragColour.r + 0.7152 * FragColour.g + 0.0722 * FragColour.b;
	FragColour = vec4(average, average, average, 1.0);
}