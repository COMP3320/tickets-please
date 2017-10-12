
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;  
out vec3 Normal;
  
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = aNormal;
}

/*#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 5) in vec3 colour;
out vec2 TexCoords;
out vec3 objectColour;
out vec4 fragNormalView;
out vec4 fragPositionView;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
	objectColour = colour;

	vec4 norm = view * model * vec4(aNormal, 0.0);
	fragNormalView = vec4(normalize(norm.xyz),0.0);
	fragPositionView = view * model * vec4(aPos, 1.0);
}*/

