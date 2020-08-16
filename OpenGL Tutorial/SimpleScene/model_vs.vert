#version 330 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

uniform mat4 model;

layout (std140) uniform Matrices
{
   mat4 view;         // offset 0          // base 64
   mat4 projection;   // offset 64         // base 64
};                    // size 128 bytes

out vec3 wNormal;
out vec3 wPosition;

void main()
{
   gl_Position = projection * view * model * vec4(Pos, 1.0f);
   wNormal = normalize(transpose(inverse(mat3(model))) * Normal);
   wPosition = vec3(model * vec4(Pos, 1.0f));
}