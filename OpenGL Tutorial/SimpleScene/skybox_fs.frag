#version 330 core

in vec3 TexCoord;

out vec4 FragColor;

uniform samplerCube skybox;

void main()
{
    
    float gamma = 2.2;
    vec4 fragcolor = texture(skybox, TexCoord);
    FragColor = vec4(pow(fragcolor.rgb,vec3(1.0/gamma)),1.0);

}