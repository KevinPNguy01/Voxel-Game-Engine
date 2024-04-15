#version 330 core
out vec4 FragColor;

in vec4 faceBrightness;
in vec3 TexCoord;

uniform sampler2DArray textureArray;


void main()
{
    FragColor = texture(textureArray, TexCoord) * faceBrightness;
}