#version 330 core

layout (location = 0) in vec2 Pos;
layout (location = 1) in vec2 TexCoords;


out vec2 TexCoords_frag;

void main()
{
  gl_Position = vec4(Pos,0.0f, 1.0f);
  TexCoords_frag = TexCoords;    
}
