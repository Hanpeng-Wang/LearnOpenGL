#version 330 core
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

out vec2 TexCoord_frag;
out vec3 wNormal;
out vec3 wPosition;

uniform mat4 model;

layout(std140) uniform Matrices
{
   mat4 view;
   mat4 projection;
};



void main()
{
    gl_Position = projection * view * model * vec4(Pos, 1.0f);
    
    TexCoord_frag = TexCoord;

    wNormal = normalize(transpose(inverse(mat3(model))) * Normal);

    wPosition = vec3(model * vec4(Pos,1.0f));
}