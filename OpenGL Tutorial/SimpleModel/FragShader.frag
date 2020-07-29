#version 330 core

in vec2 TexCoord_frag;

out vec4 FragColor;

uniform sampler2D TextureImg1;
uniform sampler2D TextureImg2;

void main()
{
    FragColor = mix(texture(TextureImg1, TexCoord_frag),texture(TextureImg2, TexCoord_frag), 0.2);
}