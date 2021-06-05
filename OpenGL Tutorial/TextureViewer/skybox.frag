#version 330 core

in vec3 TexCoords;

out vec4 FragColor;

uniform samplerCube skybox;

void main()
{
   // do gamma corrections
   float gamma = 2.2;
   vec4 TexColor = texture(skybox, TexCoords);
   FragColor = vec4(pow(TexColor.rgb, vec3(1.0/gamma)), 1.0);

}