#include "../include/Model.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Model::Model()
{}

Model::Model(std::string path)
{
	load(path);
}

void Model::Draw(Shader& shader)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shader);
	}
}

void Model::load(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);

	if (!scene)
	{
		std::cout << "Error->ASSIMP: " << importer.GetErrorString() << std::endl;
	}
	else
	{
		modelDirectory = path.substr(0, path.find_last_of("/"));

		loadNode(scene, scene->mRootNode);
	}
}

void Model::loadNode(const aiScene* scene, aiNode* node)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(loadMesh(scene, mesh));
	}

	//Handle the children
	for (int i = 0; i < node->mNumChildren; i++)
	{
		this->loadNode(scene, node->mChildren[i]);
	}
}

Mesh Model::loadMesh(const aiScene* scene, aiMesh* mesh)
{
	std::vector<Vertex> vertices;
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.textureCoordinates.x = mesh->mTextureCoords[0][i].x;
			vertex.textureCoordinates.y = mesh->mTextureCoords[0][i].y;
			
		}
		else
		{
			vertex.textureCoordinates = glm::vec2(0.0);
			std::cout << "TExtURE COORDS ARE 0" << std::endl;
		}

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.y = mesh->mNormals[i].z;

		vertices.push_back(vertex);
	}

	std::vector<unsigned int> indices;
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	std::vector<Texture> textures;
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadTexturesFromMaterial(material, aiTextureType_DIFFUSE, "diffuse");
		std::vector<Texture> specularMaps = loadTexturesFromMaterial(material, aiTextureType_SPECULAR, "specular");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadTexturesFromMaterial(aiMaterial* material, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString path;
		material->GetTexture(type, i, &path);
		//check if texture was alreaeady loaded and if so, continue to next thingy
		bool skip = false;
		for (int j = 0; j < loadedTextures.size(); j++)
		{
			if (std::strcmp(loadedTextures[j].path.data(), path.C_Str()) == 0)
			{
				textures.push_back(loadedTextures[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			texture.ID = loadTextureFromFile(path.C_Str());
			texture.type = typeName;
			texture.path = path.C_Str();
			textures.push_back(texture);
			loadedTextures.push_back(texture);
		}
	}

	return textures;
}

int Model::loadTextureFromFile(std::string fileName)
{
	unsigned int ID;
	glGenTextures(1, &ID);

	int width, height, nrChannels;
	std::string fullFilePath = modelDirectory + "/" + fileName;
	std::cout << "fullFilePath of texture: " << fullFilePath << std::endl;

	//stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(fullFilePath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		if (nrChannels == 3)
			format = GL_RGB;
		if (nrChannels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, ID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
	}
	else
	{
		std::cout << "Failed to load texture image" << std::endl;
	}
	stbi_image_free(data);

	return ID;
}