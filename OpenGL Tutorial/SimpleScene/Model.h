#pragma once
#include<glm/glm.hpp>
#include<string>
#include<vector>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include<unordered_map>
#include"Shader.h"

typedef std::unordered_map<std::string, unsigned int> TextureMap;

enum LightType {LIGHT_SOURCE_UNDEFINED,LIGHT_SOURCE_SPOT, LIGHT_SOURCE_POINT, LIGHT_SOURCE_DIRECTIONAL};

class LightSource
{
public:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	glm::vec3 lightDir;
	glm::vec3 lightPos;
	
	std::string name;
	LightType type;

	float constant;
	float linear;
	float quadratic;

	float innerCone;
	float outerCone;

	LightSource(glm::vec3 a = glm::vec3(0.2f), glm::vec3 d = glm::vec3(0.8f), glm::vec3 s = glm::vec3(1.0f));
	void SetPointlight(glm::vec3 pos, float constant, float linear, float quadratic);
	void SetDirlight(glm::vec3 lightDir);
	void SetSpotlight(glm::vec3 pos, glm::vec3 lightDir, float innerCone, float outerCone);
};

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture
{
	unsigned int id;
	std::string type;
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<Texture> textures;
	std::vector<unsigned int> indices;

	Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures, std::vector<unsigned int> indices);
	void Draw(Shader& shader);
	unsigned int GetVAO() { return this->VAO; }
	unsigned int GetVBO() { return this->VBO; }
	unsigned int GetEBO() { return this->EBO; }

private:
	void SetUp();

	//vertex array obj, vertex buffer obj and element buffer obj
	unsigned int VAO, VBO, EBO;
	
};

class Model
{
public:
	std::vector<Mesh> meshes;
	std::string directory;
	TextureMap TextureLoaded;

	std::vector<LightSource> lights;

	Model(const std::string& Modelfile)
	{
		LoadModel(Modelfile);
	}

	void Draw(Shader& shader);
private:
	
	void LoadModel(const std::string& Modelfile);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTexture(aiMaterial* material, aiTextureType type, std::string typename);
	unsigned int LoadGenTexture(const std::string& filename);
};
	