#pragma once

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
};

struct Texture {
  GLuint id;
  string type;
  //aiString path;
};

class Mesh {
public:
  /*  Mesh Data  */
  vector<Vertex> vertices;
  vector<GLuint> indices;
  vector<Texture> textures;
  
  /*  Functions  */
  // Constructor
  Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);
  
  // Render the mesh
  void Draw(Shader shader);

private:
  /*  Render data  */
  GLuint VAO, VBO, EBO;
  
  /*  Functions    */
  // Initializes all the buffer objects/arrays
  void setupMesh();
};
