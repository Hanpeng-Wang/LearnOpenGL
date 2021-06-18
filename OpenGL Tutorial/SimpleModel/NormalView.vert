#version 330 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;


layout (std140) uniform Matrices
{
   mat4 model;        // offset 0          // base 64
   mat4 view;         // offset 64          // base 64
   mat4 projection;   // offset 128         // base 64
};                    // size 192 bytes


out vec3 vNormal;

void main()
{
   gl_Position = view * model * vec4(Pos,1.0f);
   vNormal = normalize(transpose(inverse(mat3(view*model)))*Normal);
}