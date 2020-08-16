#version 330 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;


uniform mat4 model;
uniform mat4 view;


out vec3 vNormal;

void main()
{
   gl_Position = view * model * vec4(Pos,1.0f);
   vNormal = normalize(transpose(inverse(mat3(view*model)))*Normal);
}