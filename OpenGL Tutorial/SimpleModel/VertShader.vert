#version 330 core
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;
layout (location = 3) in vec3 Tangent;

out vec2 TexCoord_frag;
out vec3 wPosition;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(Pos, 1.0f);
    
    TexCoord_frag = TexCoord;

    

    // calculate TBN matrix
    vec3 N = normalize(transpose(inverse(mat3(model))) * Normal);
    vec3 T = normalize(mat3(model) * Tangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    TBN = mat3(T, B, N);


    wPosition = vec3(model * vec4(Pos,1.0f));
}