#version 330 core

in vec2 TexCoord_frag;

out vec4 FragColor;

uniform sampler2D TextureImg;


void main()
{
    FragColor = texture(TextureImg, TexCoord_frag);
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}