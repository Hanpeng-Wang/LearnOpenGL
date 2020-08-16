#version 330 core

layout (location = 0) in vec3 Pos;

out vec3 TexCoord;

layout (std140) uniform Matrices
{
   mat4 view;
   mat4 projection;
};

void main()
{
   mat4 new_view = mat4(mat3(view));
   gl_Position = projection * new_view * vec4(Pos,1.0f);
   TexCoord = Pos;
}
 