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
  vec3 PointlightColor;
   vec3 DirlightColor;
  if(Blin)
  {
   PointlightColor = ComputePointlight(true);
   DirlightColor = ComputeDirlight(true);
  }
  else
  {

   PointlightColor = ComputePointlight(false);
   DirlightColor = ComputeDirlight(false);
  }

  vec3 hdrColor = PointlightColor + DirlightColor;
  vec3 mapped = hdrColor / (hdrColor + vec3(1.0));

  FragColor = vec4(mapped, 1.0);
}


vec3 ComputeDirlight(bool Blin_Phong)
{
   vec3 ambientStren = vec3(0.0);
   vec3 diffuseStren = vec3(0.0);
   vec3 specularStren = vec3(0.0);
   vec3 Normals = normalize(wNormal);

   for(int i=0; i<DirlightNum; i++)
   {
     vec3 lightDir = normalize(-Dirlights[i].lightDir);
     ambientStren += Dirlights[i].ambient;

     float diff = max(dot(lightDir,Normals),0.0);
     diffuseStren += diff*Dirlights[i].diffuse;
     
     float spec;

     vec3 viewDir = normalize(wCameraPos - wPosition);
     if(!Blin_Phong)
     {
       vec3 reflectDir = normalize(reflect(-lightDir,Normals));
       spec = pow(max(dot(reflectDir,viewDir),0.0), material.shiness);
     }
     else
     {
       vec3 halfwayDir = normalize(viewDir + lightDir);
       spec = pow(max(dot(Normals,halfwayDir),0.0), 2*material.shiness);
     }

     if(!ifspec)
        spec = 0.0;
     specularStren += spec*Dirlights[i].specular;
   }

   vec3 Color1 = (ambientStren + diffuseStren)* texture(material.diffuseMap, TexCoord_frag).rgb;
   vec3 Color2 = specularStren*texture(material.specularMap,TexCoord_frag).rgb;
   return(Color2);
}


vec3 ComputePointlight(bool Blin_Phong)
{
   vec3 ambientStren = vec3(0.0);
   vec3 diffuseStren = vec3(0.0);
   vec3 specularStren = vec3(0.0);
    vec3 Normals = normalize(wNormal);

   for(int i=0; i<PointlightNum; i++)
   {
      vec3 lightDir = normalize(pointlights[i].lightPos - wPosition);
      float d = length(pointlights[i].lightPos - wPosition);
      float attenuation = 1.0/(pointlights[i].constant + pointlights[i].linear*d + pointlights[i].quadratic*d*d);
      ambientStren += pointlights[i].ambient*attenuation;

      float diff = max(dot(lightDir,Normals),0.0);
      diffuseStren += diff*pointlights[i].diffuse*attenuation;
      
      float spec;

      vec3 viewDir = normalize(wCameraPos - wPosition);
      if(!Blin_Phong)
      {
        
        vec3 reflectDir = normalize(reflect(-lightDir,Normals));
        spec = pow(max(dot(viewDir,reflectDir),0.0),material.shiness);
      }
      else
      {
        vec3 halfwayDir = normalize(viewDir + lightDir);
        spec = pow(max(dot(Normals,halfwayDir),0.0), 2*material.shiness);
      }
      if(!ifspec)
        spec = 0.0;
      specularStren += spec*pointlights[i].specular*attenuation;
   }

   vec3 Color1 = (ambientStren + diffuseStren)*texture(material.diffuseMap,TexCoord_frag).rgb;
   vec3 Color2 = specularStren*texture(material.specularMap,TexCoord_frag).rgb;
   return (Color2);
}
   

