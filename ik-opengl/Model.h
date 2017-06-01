/////////////////////
// Model.h
/////////////////////

#pragma once
#include "Shader.h"
#include "Mesh.h"
// Std includes
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;
// GL includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// Assimp includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
//#include <soil.h>

class Model {
public:
	Model() {}
	Model(GLchar* path) { this->LoadModel(path); }
	void Draw(Shader shader);
private:

	/* Data */
	vector<Mesh> meshes;
	string directory;
	vector <Texture> textures_loaded;

	/* Functions */
	void LoadModel(string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
  
};
