#version 330 core
layout (location = 0) in int vIndex;
layout (location = 1) in ivec4 translation;

out vec4 TexCoords;

const vec2 vertices[] = vec2[10](
	vec2(0, 0.125), //A
    vec2(-0.75,0.5), //D
    vec2(-0.75,-0.5), //C
    vec2(0,-0.875), //B
    vec2(0.75,0.5), //F
    vec2(0,0.875), //E
    vec2(-0.75,0.5), //D,
    vec2(0,-0.875), //B
    vec2(0.75,-0.5), //G
    vec2(0.75,0.5) //F
);

const float brightnesses[] = float[3](0.85, 1, 0.7);

void main()
{
    vec2 vertex = vertices[(vIndex % 4 == 0) ? 0 : vIndex/4*3 + vIndex%4];
    uvec3 textures = uvec3(translation.x >> 8, translation.x & 0xFF, translation.y);

    int texIndex = vIndex + (vIndex/4 != 0 ? 1 : 0);
    TexCoords = vec4((((texIndex % 4) & 2) >> 1) ^ ((texIndex % 4) & 1), ((texIndex % 4) & 2) >> 1, textures[vIndex/4], brightnesses[vIndex/4]);
    gl_Position = vec4(vertex, 0, 1);
}  