#version 330 core
out vec4 FragColor;

in vec4 TexCoords;

uniform sampler2DArray array;

void main()
{    
    FragColor = texture(array, TexCoords.xyz) * vec4(TexCoords.www, 1);
}