#version 330 core
layout (location = 0) in int vIndex;
layout (location = 1) in ivec4 translation;

out vec4 faceBrightness;
out vec3 TexCoord;

uniform mat4 cameraTransform;

const vec4 lightValues[] = vec4[6](
	vec4(0.85,0.85,0.85,1),
	vec4(0.85,0.85,0.85,1),
	vec4(0.55,0.55,0.55,1),
	vec4(1,1,1,1),
	vec4(0.7,0.7,0.7,1),
	vec4(0.7,0.7,0.7,1)
);

const vec3 vertices[] = vec3[24](
	vec3(1,0,0), vec3(0,0,0), vec3(0,1,0), vec3(1,1,0),
	vec3(0,0,1), vec3(1,0,1), vec3(1,1,1), vec3(0,1,1),
	vec3(0,0,0), vec3(1,0,0), vec3(1,0,1), vec3(0,0,1),
	vec3(1,1,0), vec3(0,1,0), vec3(0,1,1), vec3(1,1,1),
	vec3(0,0,0), vec3(0,0,1), vec3(0,1,1), vec3(0,1,0),
	vec3(1,0,1), vec3(1,0,0), vec3(1,1,0), vec3(1,1,1)
);

const vec2 textureVertices[] = vec2[4](
	vec2(0,1), vec2(1,1), vec2(1,0), vec2(0,0)
);

void main()
{
	int fIndex = translation.w >> 8;
	vec3 pos3 = vertices[fIndex * 4 + vIndex];

	gl_Position = cameraTransform * vec4(pos3.x + translation.x, pos3.y + (translation.y >> 16), pos3.z + translation.z, 1.0);

	int lightIndex = translation.w & 255;
	faceBrightness = lightValues[lightIndex];

	TexCoord = vec3(textureVertices[vIndex], translation.y & 255);
}