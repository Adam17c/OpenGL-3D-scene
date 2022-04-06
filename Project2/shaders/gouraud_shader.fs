#version 330 core
out vec4 FragColor;
uniform sampler2D ourTexture;

in vec2 TexCoords;
in vec3 LightingColor; 

vec3 objectColor = texture(ourTexture, TexCoords).xyz;;

// rysuj g³adki obiekt
uniform bool drawSmoothObject;
uniform vec3 smoothObjectColor;

void main()
{
    if (drawSmoothObject) objectColor = smoothObjectColor;
   FragColor = vec4(LightingColor * objectColor, 1.0);
}