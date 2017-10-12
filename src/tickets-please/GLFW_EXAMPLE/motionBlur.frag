#version 330 core
out vec4 FragColour;

in vec2 TexCoords;

uniform sampler2D screenTextures[5];
uniform int mode;
// 0 - normal
// 1 - inverse
// 2 - motion blur

void main(){
	switch(mode){
		// Normal mode
		case 0:
			FragColour = texture(screenTextures[0], TexCoords);
			break;
		// Inversion mode - I
		case 1:
			FragColour = vec4(1 - vec3(texture(screenTextures[0], TexCoords)), 1.0);
			break;
		// Grayscale mode - G
		case 2:
			FragColour = texture(screenTextures[0], TexCoords);
			float average = 0.2126 * FragColour.r + 0.7152 * FragColour.g + 0.0722 * FragColour.b;
			FragColour = vec4(average, average, average, 1.0);
			break;
		// Motion blur - M
		case 3:
			FragColour = vec4(vec3(texture(screenTextures[0], TexCoords)), 0.0)
						+ vec4(vec3(texture(screenTextures[1], TexCoords)), 0.3)
						+ vec4(vec3(texture(screenTextures[2], TexCoords)), 0.2)
						+ vec4(vec3(texture(screenTextures[3], TexCoords)), 0.1)
						+ vec4(vec3(texture(screenTextures[4], TexCoords)), 1.0);

			break;
		default:
			break;
	}
	
}