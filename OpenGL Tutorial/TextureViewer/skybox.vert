#version 330 core

layout (location = 0) in vec3 Pos;

out vec3 TexCoords;

layout(std140) uniform Matrices
{
   mat4 view;
   mat4 projection;
};

void main()
{
   mat4 view_NoTrans = mat4(mat3(view));
   gl_Position = projection * view_NoTrans * vec4(Pos, 1.0);
   gl_Position = gl_Position.xyww;
   TexCoords = Pos;
}