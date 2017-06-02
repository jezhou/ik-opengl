//
//  Model.cpp

#include "Model.h"

Model::Model(GLchar* path){
  this->LoadModel(path);
}

void Model::Draw(Shader shader) {
  for(GLuint i = 0; i < this->meshes.size(); i++)
    this->meshes[i].Draw(shader);
}

void Model::LoadModel(string path){
  
  // Read file via ASSIMP
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
  
  // Check for errors
  if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
  {
    cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
    return;
  }
  
  // Retrieve the directory path of the filepath
  this->directory = path.substr(0, path.find_last_of('/'));
  
  // Process ASSIMP's root node recursively
  this->ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene){
  
  // Process each mesh located at the current node
  for(GLuint i = 0; i < node->mNumMeshes; i++)
  {
    // The node object only contains indices to index the actual objects in the scene.
    // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    this->meshes.push_back(this->ProcessMesh(mesh, scene));
  }
  // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
  for(GLuint i = 0; i < node->mNumChildren; i++)
  {
    this->ProcessNode(node->mChildren[i], scene);
  }
}

Mesh Model::ProcessMesh (aiMesh* mesh, const aiScene* scene){
  
  // Data to fill
  vector<Vertex> vertices;
  vector<GLuint> indices;
  vector<Texture> textures;
  
  // Walk through each of the mesh's vertices
  for(GLuint i = 0; i < mesh->mNumVertices; i++){
    
    Vertex vertex;
    glm::vec3 vector; // assimp uses its own vector class, so this is a placeholder
    
    // Positions
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.Position = vector;
    
    // Normals
    vector.x = mesh->mNormals[i].x;
    vector.y = mesh->mNormals[i].y;
    vector.z = mesh->mNormals[i].z;
    vertex.Normal = vector;
    
    // Texture Coordinates ( None )
    vertex.TexCoords = glm::vec2(0.0f, 0.0f);
    
    vertices.push_back(vertex);
  }
  
  // Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
  for(GLuint i = 0; i < mesh->mNumFaces; i++)
  {
    aiFace face = mesh->mFaces[i];
    
    // Retrieve all indices of the face and store them in the indices vector
    for(GLuint j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }
  
  // Return a mesh object created from the extracted mesh data
  return Mesh(vertices, indices, textures);
}

