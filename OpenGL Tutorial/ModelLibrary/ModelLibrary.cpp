// ModelLibrary.cpp : Defines the functions for the static library.
//

#include "Model.h"

#include "Model.h"
#include<iostream>
#include<glad/glad.h>
#include "stb_image.h"
#include <fstream>
#include <iostream>
#include <sstream>


Shader::Shader(std::string vertShader, std::string fragShader)
{
	std::ifstream vertStream;
	std::ifstream fragStream;

	std::string vertCode;
	std::string fragCode;

	//set exceptions thrown
	vertStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// read vertex shader
		vertStream.open(vertShader);

		std::stringstream vShaderStream;
		std::stringstream fShaderStream;

		vShaderStream << vertStream.rdbuf();

		// read shader shader
		fragStream.open(fragShader);

		fShaderStream << fragStream.rdbuf();


		vertStream.close();
		fragStream.close();

		vertCode = vShaderStream.str();
		fragCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}



	// linking and compiling shaders
	unsigned int vert = glCreateShader(GL_VERTEX_SHADER);
	unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER);


	const char* vertPointer = vertCode.c_str();
	const char* fragPointer = fragCode.c_str();

	glShaderSource(vert, 1, &vertPointer, NULL);
	glShaderSource(frag, 1, &fragPointer, NULL);

	// compiling
	int success;
	char infoLog[512];

	glCompileShader(vert);
	glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vert, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	glCompileShader(frag);
	glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(frag, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	ProgramID = glCreateProgram();
	glAttachShader(ProgramID, vert);
	glAttachShader(ProgramID, frag);
	glLinkProgram(ProgramID);

	glGetProgramiv(ProgramID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ProgramID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vert);
	glDeleteShader(frag);
}

void Shader::SetUpGeometryShader(std::string geomShader)
{
	std::ifstream geomStream;
	std::string geomCode;

	geomStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		geomStream.open(geomShader);

		std::stringstream geomStringStream;
		geomStringStream << geomStream.rdbuf();

		geomStream.close();
		geomCode = geomStringStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	unsigned int geom = glCreateShader(GL_GEOMETRY_SHADER);
	const char* geomptr = geomCode.c_str();

	glShaderSource(geom, 1, &geomptr, NULL);

	int success;
	char infoLog[512];

	glCompileShader(geom);
	glGetShaderiv(geom, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(geom, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	glAttachShader(ProgramID, geom);
	glLinkProgram(ProgramID);

	glGetProgramiv(ProgramID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ProgramID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(geom);
}

void Shader::Use()
{
	glUseProgram(ProgramID);
}

void Shader::SetUniformInt(std::string name, int n)
{
	glUniform1i(glGetUniformLocation(ProgramID, name.c_str()), n);
}

void Shader::SetUniformMat4(std::string name, bool transpose, const float* value)
{
	glUniformMatrix4fv(glGetUniformLocation(ProgramID, name.c_str()), 1, transpose, value);
}

void Shader::SetUniformVec3(std::string name, const float* value)
{
	glUniform3fv(glGetUniformLocation(ProgramID, name.c_str()), 1, value);
}

void Shader::SetUniformf(std::string name, float f)
{
	glUniform1f(glGetUniformLocation(ProgramID, name.c_str()), f);
}


LightSource::LightSource(glm::vec3 a, glm::vec3 d, glm::vec3 s) : ambient(a), diffuse(d), specular(s)
{
	this->lightPos = glm::vec3(0.0f);
	this->lightDir = glm::vec3(0.0f);;

	this->constant = 1.0;
	this->linear = 0.09f;
	this->quadratic = 0.032f;

	this->innerCone = 0.0f;
	this->outerCone = 0.0f;

	this->type = LightType::LIGHT_SOURCE_UNDEFINED;
	this->name = "";
}

void LightSource::SetPointlight(glm::vec3 pos, float constant, float linear, float quadratic)
{
	this->lightPos = pos;
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
	this->type = LightType::LIGHT_SOURCE_POINT;
}

void LightSource::SetDirlight(glm::vec3 lightDir)
{
	this->lightDir = lightDir;
	this->type = LightType::LIGHT_SOURCE_DIRECTIONAL;
}

void LightSource::SetSpotlight(glm::vec3 pos, glm::vec3 lightDir, float innerCone, float outerCone)
{
	this->lightPos = pos;
	this->lightDir = lightDir;
	this->innerCone = innerCone;
	this->outerCone = outerCone;
	this->type = LightType::LIGHT_SOURCE_SPOT;
}


Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<Texture>& textures, const std::vector<unsigned int>& indices)
{
	this->vertices = vertices;
	this->textures = textures;
	this->indices = indices;

	SetUp();
}
Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<Texture>&& textures, std::vector<unsigned int>&& indices): vertices(vertices), textures(textures), indices(indices)
{
	SetUp();
}

void Mesh::SetUp()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	//Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	//Normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(1);
	//TexCoords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader)
{


	for (int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);

		if (textures[i].type == "texture_diffuse")
			shader.SetUniformInt("material.diffuseMap", i);

		if (textures[i].type == "texture_specular")
			shader.SetUniformInt("material.specularMap", i);

	}

	shader.SetUniformInt("material.shiness", 333);

	//bind vertex array
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

}


void Model::LoadModel(const std::string& Modelfile)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(Modelfile, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
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
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
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
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
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
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
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
		std::vector<Texture> normal = LoadMaterialTexture(material, aiTextureType_NORMALS, "texture_normals");
		textures.insert(textures.end(), diffuse.begin(), diffuse.end());
		textures.insert(textures.end(), specular.begin(), specular.end());
		textures.insert(textures.end(), normal.begin(), normal.end());
	}

	return Mesh(std::move(vertices), std::move(textures), std::move(indices));

}

std::vector<Texture> Model::LoadMaterialTexture(aiMaterial* material, aiTextureType type, std::string type_name)
{
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString filename;
		material->GetTexture(type, i, &filename);
		Texture tex;

		if (TextureLoaded.count(filename.C_Str()) == 0)
		{

			tex.id = LoadGenTexture(filename.C_Str(),type);
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

unsigned int Model::LoadGenTexture(const std::string& filename,aiTextureType type)
{
	std::string fullpath = directory + "/" + filename;
	unsigned int texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	//set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nChannels;
	unsigned char* data = stbi_load(fullpath.c_str(), &width, &height, &nChannels, 0);

	if (data == NULL)
	{
		std::cout << "ERROR::TEXTURE::FAILED TO LOAD TEXTURE" << std::endl;
		std::cout << fullpath << std::endl;
	}

	GLenum format; 
	GLenum Internalformat;

	switch (nChannels)
	{
	case 1:
		format = GL_RED;
		Internalformat = GL_RED;
		break;
	case 3:
		format = GL_RGB;
		if (type == aiTextureType_DIFFUSE) Internalformat = GL_RGB;
		else Internalformat = GL_RGB;	
		break;
	case 4:
		format = GL_RGBA;
		if (type == aiTextureType_DIFFUSE) Internalformat = GL_RGBA;
		else Internalformat = GL_RGBA;
		break;

	default:
		format = GL_RGB;
		Internalformat = GL_RGB;
		break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, Internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data); // free the memory of image

	return texture;
}

void Model::Draw(Shader& shader)
{

	for (int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);

}
