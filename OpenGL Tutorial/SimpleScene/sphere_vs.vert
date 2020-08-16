#version 330 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;
layout (location = 3) in mat4 offset;


layout (std140) uniform Matrices
{
   mat4 view;        
   mat4 projection;
};

uniform mat4 Scale;
uniform mat4 Rotation;

out vec3 wNormal;
out vec3 wPosition;


void main()
{
   mat4 model = Rotation*offset*Scale;
   wNormal = normalize(transpose(inverse(mat3(model)))*Normal);
   wPosition = vec3(model * vec4(Pos,1.0f));

   gl_Position = projection*view*model*vec4(Pos,1.0f);

}