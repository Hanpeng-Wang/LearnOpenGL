#version 330 core

layout(triangles) in;
layout (line_strip,max_vertices = 2) out;

in vec3 vNormal[];
layout (std140) uniform Matrices
{
   mat4 model;        // offset 0          // base 64
   mat4 view;         // offset 64          // base 64
   mat4 projection;   // offset 128         // base 64
};  

vec4 ComputeNormalV(in vec3 Normal, in vec4 Pos);

void main()
{
    for(int i=0; i<3;i++)
    {
     gl_Position = projection * gl_in[i].gl_Position;
     EmitVertex();
    
     gl_Position = projection*ComputeNormalV(vNormal[i],gl_in[i].gl_Position);
     EmitVertex();
     EndPrimitive();
    }
}

vec4 ComputeNormalV(in vec3 Normal, in vec4 Pos)
{
    vec4 new_pos = Pos + vec4(Normal,0.0f)*0.4;
    return new_pos;
}