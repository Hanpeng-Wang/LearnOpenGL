#version 330 core

in vec2 TexCoords_frag;

out vec4 FragColor;

uniform sampler2D QuadTexture;

void main()
{
    FragColor = texture(QuadTexture,TexCoords_frag);
}

