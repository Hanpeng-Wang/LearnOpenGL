#include "Model.h"
#include<iostream>
#include<glad/glad.h>
#include "stb_image.h"


LightSource::LightSource(glm::vec3 a, glm::vec3 d, glm::vec3 s): ambient(a), diffuse(d), specular(s)
{
	this->lightPos = glm::vec3(0.0f);
	this->lightDir = glm::vec3(0.0f);;

	this->constant = 1.0;
	this->linear = 0.09f;
	this->quadratic = 0.032f;

	this->innerCone = 0.0f;
	this->outerCone = 0.0f;

	this->type = LIGHT_SOURCE_UNDEFINED;
	this->name = "";
}

void LightSource::SetPointlight(glm::vec3 pos, float constant, float linear, float quadratic)
{
	this->lightPos = pos;
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
	this->type = LIGHT_SOURCE_POINT;
}

void LightSource::SetDirlight(glm::vec3 lightDir)
{
	this->lightDir = lightDir;
}

void LightSource::SetSpotlight(glm::vec3 pos, glm::vec3 lightDir, float innerCone, float outerCone)
{
	this->lightPos = pos;
	this->lightDir = lightDir;
	this->innerCone = innerCone;
	this->outerCone = outerCone;
}


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Texture> textures, std::vector<unsigned int> indices)
{
	this->vertices = vertices;
	this->textures = textures;
	this->indices = indices;

	SetUp();
}

void Mesh::SetUp()
{
	glGenVertexArrays(1,&VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(Vertex),&vertices[0], GL_STATIC_DRAW);

	//Position
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)0);
	glEnableVertexAttribArray(0);
	//Normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(1);
	//TexCoords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(unsigned int),&indices[0],GL_STATIC_DRAW);
	
	glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader)
{


	for (int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D,textures[i].id);

		if(textures[i].type == "texture_diffuse")
			shader.SetUniformInt("material.diffuseMap",i);

		if (textures[i].type == "texture_specular")
			shader.SetUniformInt("material.specularMap", i);
		
	}

	shader.SetUniformInt("material.shiness", 32);

	//bind vertex array
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,0);
	glBindVertexArray(0);

}


void Model::LoadModel(const std::string& Modelfile)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(Modelfile,aiProcess_Triangulate|aiProcess_FlipUVs|aiProcess_GenNormals);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}

	directory = Modelfile.substr(0, Modelfile.find_last_of('/'));

	ProcessNode(scene->mRootNode, scene);
	
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	//process all the meshes in current node
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));		
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}

}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<Texture> textures;
	std::vector<unsigned int> indices;

	//process vertices in the mesh
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex V;
		V.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		V.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

		if (mesh->mTextureCoords[0])
			V.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		else
			V.TexCoords = glm::vec2(0.0f);
		
		vertices.push_back(V);
	}

	//process indices
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	//process materials(textures)
	if (mesh->mMaterialIndex >= 0)
	{
		
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		//aiString name;
		//material->Get(AI_MATKEY_NAME,name);
		//std::cout << "MATERIAL::NAME::" << name.C_Str() << std::endl;

		std::vector<Texture> diffuse = LoadMaterialTexture(material, aiTextureType_DIFFUSE, "texture_diffuse");
		std::vector<Texture> specular = LoadMaterialTexture(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), diffuse.begin(), diffuse.end());
		textures.insert(textures.end(), specular.begin(), specular.end());
	}

	return Mesh(vertices, textures, indices);

}

std::vector<Texture> Model::LoadMaterialTexture(aiMaterial* material, aiTextureType type, std::string type_name)
{
	std::vector<Texture> textures;

	for (int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString filename;
		material->GetTexture(type, i, &filename);
		Texture tex;
		
		if (TextureLoaded.count(filename.C_Str()) == 0)
		{
			
			tex.id = LoadGenTexture(filename.C_Str());
			tex.type = type_name;
			textures.push_back(tex);
			TextureLoaded[filename.C_Str()] = tex.id;
		}
		else
		{
			tex.id = TextureLoaded[filename.C_Str()];
			tex.type = type_name;
			textures.push_back(tex);
		}
	}

	return textures;
}

unsigned int Model::LoadGenTexture(const std::string& filename)
{
	std::string fullpath = directory + "/" + filename;
	unsigned int texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,texture);

	//set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nChannels;
	unsigned char* data = stbi_load(fullpath.c_str(),&width,&height,&nChannels,0);

	if (data == NULL)
	{
		std::cout << "ERROR::TEXTURE::FAILED TO LOAD TEXTURE" << std::endl;
		std::cout << fullpath << std::endl;
		
	}

	GLenum format;

	switch (nChannels)
	{
	case 1:
		format = GL_RED;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;

	default:
		format = GL_RGB;
		break;
	}


	glTexImage2D(GL_TEXTURE_2D,0,format,width,height,0,format,GL_UNSIGNED_BYTE,data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data); // free the memory of image

	return texture;
}

void Model::Draw(Shader& shader)
{
	
	for (int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);

}