#version 330 core

#define MAX_NUM_LIGHTS 10

struct Material
{
  sampler2D diffuseMap;
  sampler2D specularMap;
  float shiness;
};

struct PointLight
{
  vec3 lightPos;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};


struct DirectionLight
{
  vec3 lightDir;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

in vec2 TexCoord_frag;
in vec3 wNormal;
in vec3 wPosition;

out vec4 FragColor;

uniform vec3 wCameraPos;
uniform Material material;
uniform bool Blin;
uniform int w;
uniform bool ifspec;

uniform int PointlightNum;
uniform PointLight pointlights[MAX_NUM_LIGHTS];
uniform int DirlightNum;
uniform DirectionLight Dirlights[MAX_NUM_LIGHTS];


vec3 ComputeDirlight(bool Blin_Phong);
vec3 ComputePointlight(bool Blin_Phong);

void main()
{
  if(Blin)
  {
   vec3 PointlightColor = ComputePointlight(true);
   vec3 DirlightColor = ComputeDirlight(true);
   FragColor = vec4(PointlightColor + DirlightColor,1.0f);
  }
  else
  {

   vec3 PointlightColor = ComputePointlight(false);
   vec3 DirlightColor = ComputeDirlight(false);
   FragColor = vec4(PointlightColor + DirlightColor,1.0f);
  }

  if(FragColor.r >1.0 ||FragColor.g >1.0||FragColor.b >1.0)
      FragColor = vec4(0,1,0,1);

   //FragColor = texture(material.diffuseMap,TexCoord_frag);
}


vec3 ComputeDirlight(bool Blin_Phong)
{
   vec3 ambientStren = vec3(0.0);
   vec3 diffuseStren = vec3(0.0);
   vec3 specularStren = vec3(0.0);

   for(int i=0; i<DirlightNum; i++)
   {
     vec3 lightDir = normalize(-Dirlights[i].lightDir);
     ambientStren += Dirlights[i].ambient;

     float diff = max(dot(lightDir,wNormal),0.0);
     diffuseStren += diff*Dirlights[i].diffuse;
     
     float spec;

     if(!Blin_Phong)
     {
       vec3 reflectDir = reflect(-lightDir,wNormal);
       vec3 viewDir = normalize(wCameraPos - wPosition);
       spec = pow(max(dot(reflectDir,viewDir),0.0), material.shiness);
     }
     else
     {
       vec3 viewDir = normalize(wCameraPos - wPosition);
       vec3 halfwayDir = normalize(viewDir + lightDir);
       spec = pow(max(dot(wNormal,halfwayDir),0.0), 2*material.shiness);
     }

     if(!ifspec)
        spec = 0.0;
     specularStren += spec*Dirlights[i].specular;
   }

   vec3 Color1 = (ambientStren + diffuseStren)* texture(material.diffuseMap, TexCoord_frag).rgb;
   vec3 Color2 = specularStren*texture(material.specularMap,TexCoord_frag).rgb;
   //Color1 = vec3(0,0,0);
   return(Color1 + Color2);
}


vec3 ComputePointlight(bool Blin_Phong)
{
   vec3 ambientStren = vec3(0.0);
   vec3 diffuseStren = vec3(0.0);
   vec3 specularStren = vec3(0.0);

   for(int i=0; i<PointlightNum; i++)
   {
      vec3 lightDir = normalize(pointlights[i].lightPos - wPosition);
      float d = length(pointlights[i].lightPos - wPosition);
      float attenuation = 1.0/(pointlights[i].constant + pointlights[i].linear*d + pointlights[i].quadratic*d*d);
      ambientStren += pointlights[i].ambient*attenuation;

      float diff = max(dot(lightDir,wNormal),0.0);
      diffuseStren += diff*pointlights[i].diffuse*attenuation;
      
      float spec;

      if(!Blin_Phong)
      {
        vec3 viewDir = normalize(wCameraPos - wPosition);
        vec3 reflectDir = reflect(-lightDir,wNormal);
        spec = pow(max(dot(viewDir,reflectDir),0.0),material.shiness);
      }
      else
      {
        vec3 viewDir = normalize(wCameraPos - wPosition);
        vec3 halfwayDir = normalize(viewDir + lightDir);
        spec = pow(max(dot(wNormal,halfwayDir),0.0), 2*material.shiness);
      }
      if(!ifspec)
        spec = 0.0;
      specularStren += spec*pointlights[i].specular*attenuation;
   }

   vec3 Color1 = (ambientStren + diffuseStren)*texture(material.diffuseMap,TexCoord_frag).rgb;
   vec3 Color2 = specularStren*texture(material.specularMap,TexCoord_frag).rgb;
   //Color1 = vec3(0,0,0);
   return (Color1 + Color2);
}
   

