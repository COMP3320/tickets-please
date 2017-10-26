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

uniform mat4 view, proj, previousView;

void main(){
    // get current world space position:
    vec3 viewRay = vec3(view[0][3], view[1][3], view[2][3]);
    vec3 current = viewRay * texture(screenTexture0, TexCoords).r;
    current = mat3(view) * current;

        // get previous screen space position:
    vec4 previous = previousView * vec4(0.5, 0.5, -0.5, 1.0);
    previous.xyz /= previous.w;
    previous.xy = previous.xy * 0.5 + 0.5;

    vec2 blurVec = previous.xy - TexCoords;

    // perform blur:
    vec4 result = texture(screenTexture0, TexCoords);
    int nSamples = 30;
    for (int i = 1; i < nSamples; ++i) {
        // get offset in range [-0.5, 0.5]:
        vec2 offset = blurVec * (float(i) / float(nSamples - 1) - 0.5);
 
        // sample & add to result:
        result += texture(screenTexture0, TexCoords + offset);
    }

    result /= float(nSamples);

    FragColour = 0.9 * texture(screenTexture0, TexCoords) + 0.3 * vec4((1 - vec3(result)), 1.0);
    /*
        0.1 * vec4(vec3(texture(screenTexture0, TexCoords)), 0.01) +
        0.1 * vec4(vec3(texture(screenTexture1, TexCoords)), 0.01) +
        0.1 * vec4(vec3(texture(screenTexture2, TexCoords)), 0.01) +
        0.1 * vec4(vec3(texture(screenTexture3, TexCoords)), 0.01) +
        0.1 * vec4(vec3(texture(screenTexture4, TexCoords)), 0.01) +
        0.1 * vec4(vec3(texture(screenTexture5, TexCoords)), 0.01) +
        0.1 * vec4(vec3(texture(screenTexture6, TexCoords)), 0.01) +
        0.1 * vec4(vec3(texture(screenTexture7, TexCoords)), 0.01);
    */
}