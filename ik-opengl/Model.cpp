/////////////////////
// Mesh.cpp
/////////////////////

#include "Model.h"

/////////////////////////////////////////////////////

void Model::Draw(Shader shader) {
	for (GLuint i = 0; i < this->meshes.size(); i++) {
		this->meshes[i].Draw(shader);
	}
}

void Model::LoadModel(string path) {
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		cout << "ERROR::ASSIMP" << import.GetErrorString() << endl;
	}
	this->directory = path.substr(0, path.find_last_not_of('/'));
	this->ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene) {
	// Process all the node meshes (if any)
	for (GLuint i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->ProcessMesh(mesh, scene));
	}

	// Recursively process all of the non mesh nodes to search for meshes
	for (GLuint i = 0; i < node->mNumChildren; i++) {
		this->ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
	// Relevent info we're looking for
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

	// Process vertices
	for (GLuint i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		glm::vec3 vector;
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		// textures
		if (mesh->mTextureCoords[0]) { // check if it has texture coords
			glm::vec2 texVec;
			texVec.x = mesh->mTextureCoords[0][i].x;
			texVec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = texVec;
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		vertices.push_back(vertex);
	}

	// Process the faces
	for (GLuint i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// Process materials
	if (mesh->mMaterialIndex >= 0) { // check if materials exist
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		/* Note: Using the naming convention defined in our Shader class */
		// diffuse
		vector<Texture> diffuseMaps = this->LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// speculars
		vector<Texture> specularMaps = this->LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	// Creates a Mesh object with the extracted data and returns it
	return Mesh(vertices, indices, textures);
}

vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName) {
	vector<Texture> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if the texture has already been loaded
		GLboolean skip = false;
		for (GLuint j = 0; j < textures_loaded.size(); j++) {
			if (strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0) { // meaning a texture with same filepath has been loaded
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		/*
		if (!skip) {
			Texture texture;
			texture.id = this->TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			this->textures_loaded.push_back(texture);
		}
		*/
	}
	return textures;
}
