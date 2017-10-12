
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
  
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;
	FragColor = vec4(result, 1.0);
}

/*#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 objectColour;

in vec4 fragNormalView;
in vec4 fragPositionView;

uniform sampler2D tex;
uniform vec4 lightPos;
uniform vec4 lightColour;

void main()
{    
    FragColor = texture(tex, TexCoords);
  //Compute direction of light from frag position in view space
	vec3 lightDisplacement = lightPos.xyz - fragPositionView.xyz;
	
	//Distance to light
	float d = length(lightDisplacement);
	
	//Normalised light vector
	vec3 normalisedLightDisp = normalize(lightDisplacement);
	
	//Lambertian light equation for diffuse component
	vec4 diffuse = dot(normalisedLightDisp, fragNormalView.xyz) * lightColour;
	
	//Compute light reflection using (negate normalisedLightDisp because the light comes from the light to the surface)
	vec3 reflection = reflect(-normalisedLightDisp, fragNormalView.xyz);
	
	//Specular component.
	float shininess = 100;
	float specular_intensity = clamp(dot(reflection, -normalize(fragPositionView.xyz)),0,1);
	vec4 specular = pow(specular_intensity, shininess) * lightColour;
	
	//Ambient
	vec4 ambient = vec4(0.3,0.3,0.3,0.01);
    
    //Final total colour including diffuse, specular, ambient, falloff (with 1/d^2), texture and colour
	//FragColor =  ((diffuse + specular) * (1/(d*d)) + ambient) * texture(tex, TexCoords) * vec4(objectColour, 1.0);
}*/

