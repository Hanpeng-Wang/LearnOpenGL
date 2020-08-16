#version 330 core

in vec3 wNormal;
in vec3 wPosition;

out vec4 FragColor;

uniform vec3 CameraPos;
uniform samplerCube skybox;

void main()
{
   //float ratio = 1.00/1.52;
   vec3 viewDir = normalize(wPosition - CameraPos);
   vec3 r = reflect(viewDir,wNormal);
   //vec3 r = refract(viewDir,wNormal,ratio);
   FragColor = texture(skybox,r);
}
