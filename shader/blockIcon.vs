#version 330 core
layout (location = 0) in int vIndex;
layout (location = 1) in ivec4 translation;

out vec4 TexCoords;

uniform float slotSize;
uniform float aspectRatio;

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

const float brightnesses[] = float[3](0.75, 1, 0.5);

void main()
{
    vec2 vertex = vertices[(vIndex % 4 == 0) ? 0 : vIndex/4*3 + vIndex%4] * vec2(slotSize/aspectRatio, slotSize) * 0.85;
    vertex.y -= 1 - slotSize*1.1;
    vertex.x += (-4+translation.z)*2*slotSize/aspectRatio;
    uvec3 textures = uvec3(translation.x >> 8, translation.x & 0xFF, translation.y);

    int texIndex = vIndex + (vIndex/4 != 0 ? 1 : 0);
    TexCoords = vec4((((texIndex % 4) & 2) >> 1) ^ ((texIndex % 4) & 1), ((texIndex % 4) & 2) >> 1, textures[vIndex/4], brightnesses[vIndex/4]);
    gl_Position = vec4(vertex, 0, 1);
}  