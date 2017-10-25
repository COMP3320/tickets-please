

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Colour;

in vec4 fragNormalView;
in vec4 fragPositionView;

uniform sampler2D tex;
uniform vec4 light_position[4];
uniform vec4 light_colour[4];

void main()
{    
    //FragColor = texture(tex, TexCoords);
  //Compute direction of light from frag position in view space
	int i;
	for(i = 0; i < 4; ++i){
		vec3 lightDisplacement = light_position[i].xyz - fragPositionView.xyz;
	
		//Distance to light
		float d = length(lightDisplacement);
	
		//Normalised light vector
		vec3 normalisedLightDisp = normalize(lightDisplacement);
	
		//Lambertian light equation for diffuse component
		vec4 diffuse = dot(normalisedLightDisp, fragNormalView.xyz) * light_colour[i];
	
		//Compute light reflection using (negate normalisedLightDisp because the light comes from the light to the surface)
		vec3 reflection = reflect(-normalisedLightDisp, fragNormalView.xyz);
	
		//Specular component.
		float shininess = 10;
		float specular_intensity = max(dot(reflection, -normalize(fragPositionView.xyz)), 0.0);
		vec4 specular = pow(specular_intensity, shininess) * light_colour[i];
	
		//Ambient
		vec4 ambient = vec4(0.9f, 0.9, 0.9f, 0.9f);
    
		//Final total colour including diffuse, specular, ambient, falloff (with 1/d^2), texture and colour
		FragColor = ((diffuse + specular) * (1/(d*d)) + ambient) * texture(tex, TexCoords);
	}
}

