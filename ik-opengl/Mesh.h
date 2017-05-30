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
#include <unordered_map>
#include <queue>
#include <set>
#include <stack>

// For aiFace
#include <assimp/scene.h>

#include "Shader.h"

struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
  glm::mat4 Q;
  float Error;         // The Vertex error described in Garland 97
  bool deleted;        // In our implementation, one vertex is deleted in the collapse.
                       // We need to mark it as so.
};

struct Texture {
  GLuint id;
  string type;
  //aiString path;
};

struct Edge {
  Vertex v1;   // Using pointers because we always want to ref
  Vertex v2;   // the original vertices
  float Error;   // The cost of collapsing a pair; v1->Error + v2->Error
  pair<GLint, GLint> endpoints;
};

class Compare
{
public:
  bool operator() (Edge v1, Edge v2)
  {
    return v1.Error < v2.Error;
  }
};

class Mesh {
public:
  /*  Mesh Data  */
  vector<Vertex> vertices;
  vector<Vertex> old_vertices; // Needed for geomorphing
  std::multiset<Edge, Compare> edges;
  vector<pair<aiFace, bool>> indices;
  vector<GLint> expanded_indices; // created everytime we setup the mesh
  vector<Texture> textures;
  unordered_map<GLint, vector<pair<int,aiFace>>> adj_list;
  unordered_map<GLint, vector<GLint>> edge_check;
  
  // face, face_i i, j, v1_i, v2_i (need this to delegate reassigning until after lerping)
  vector<tuple<aiFace, int, int, int, int, int>> anchor;
  vector<int> delete_faces_after_lerp;
  
  // Old vertex 1 data, index of vertex 2, list of indices of removed faces,
  // list of indices of adj_list to add faces back to, list of faces we need to "reanchor",
  // old vector used in v1 of adj list (v1 and v2 get added together so you should revert it back)
  // Todo: Probably better just to make this a data structure instead of a fat tuple lol...
  stack<tuple<Vertex, Vertex, GLint, GLint, set<GLint>, vector<pair<GLint, GLint>>, set<pair<GLint, GLint>>, vector<pair<int,aiFace>>>> collapse_stack;
  stack<pair<int, Edge>> edge_stack;


  float lerp = 1.0f; // linear interpolation constant needed for geomorphing
  unsigned long num_faces = 0;

  /*  Functions  */
  // Constructor
  Mesh(vector<Vertex> vertices, vector<pair<aiFace, bool>> indices, vector<Texture> textures);

  // Render the mesh
  void Draw(Shader shader);
  void MeshCollapse();
  void UpdateEdges(Edge collapsed, int update_id);
  void EdgeCollapse(GLint v1, GLint v2);
  void VertexSplit();
  void setupQs();
  void setupEdges();
  void setupMesh();
  glm::vec4 plane_eq(Vertex p1, Vertex p2, Vertex p3);

private:
  /*  Render data  */
  GLuint VAO, old_VBO, VBO, EBO;
  bool meshUpdated;


  /*  Functions    */
  // Initializes all the buffer objects/arrays
  pair<int,int> ChooseVertices();

  /* Storing and loading our decimation data */
  char* dataFilePath = "/Users/Robb/cse-163-pa2/data.txt";
};
