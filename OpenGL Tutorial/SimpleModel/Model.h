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
	
