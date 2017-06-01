/////////////////////
// Mesh.h
/////////////////////

#pragma warning(disable : 4201)

#include "Shader.h"

#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
// GL includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture {
	GLuint id;
	string type;
	aiString path;
};

class Mesh {
public:
	/* Data */
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

	/* Functions*/
	Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);
	void Draw(Shader shader);
private:
	/* Render Data */
	GLuint VAO, VBO, EBO;
	/* Functions */
	void SetupMesh();
};
