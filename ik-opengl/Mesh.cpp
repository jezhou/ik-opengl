//
//  Mesh.cpp
//

#include "Mesh.h"

Mesh::Mesh(vector<Vertex> vertices, vector<pair<aiFace, bool>> indices, vector<Texture> textures){
  this->vertices = vertices;
  this->old_vertices = vertices;
  this->indices = indices;
  this->textures = textures;

  // Clear our decimation data
  fstream dataFile(dataFilePath, std::ofstream::out | std::ofstream::trunc);
  dataFile.close();

  // Construct the vertex-to-face adjacency list
  for(int i = 0; i < indices.size(); ++i) {
    aiFace current = indices[i].first;
    for(int j = 0; j < current.mNumIndices; ++j) {
      unsigned int index = current.mIndices[j];
      adj_list[index].push_back(make_pair(i, current));
    }
  }

  // Now that we have all the required data, set the vertex buffers and its attribute pointers.
  this->setupQs();
  this->setupEdges();
  num_faces = indices.size();

  this->setupMesh();
  meshUpdated = false;

}

void Mesh::Draw(Shader shader)  {

  lerp += 0.3f;
  
  if(lerp >= 1.0f) lerp = 1.0f;

  // Bind appropriate textures
  GLuint diffuseNr = 1;
  GLuint specularNr = 1;

  // Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
  glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 16.0f);

  // Draw mesh
  glBindVertexArray(this->VAO);
  glDrawElements(GL_TRIANGLES, this->expanded_indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  
  // This is where we need to combine the vertices together. the reason we do it here is because the frickin geomorph needs to
  // finish animating before we do anything funny lol
  
  if(lerp >= 1.0f) {
    for(int x = 0; x < anchor.size(); ++x) {
      auto tup = anchor[x];
      aiFace face = get<0>(tup);
      int face_i = get<1>(tup);
      int i      = get<2>(tup);
      int j      = get<3>(tup);
      int v1_i   = get<4>(tup);
      int v2_i   = get<5>(tup);
      
      vertices[v2_i].deleted = true;
      this->indices[face_i].first.mIndices[j] = v1_i; // This updates v2 to use v1 (which is the midpoint)
    }
    
    for(int x = 0; x < delete_faces_after_lerp.size(); ++x) {
      indices[delete_faces_after_lerp[x]].second = false;
    }
    
    delete_faces_after_lerp.clear();
    anchor.clear();
  }

}

void Mesh::setupMesh(){

  int count = 0;

  // Expand the face data we have, but check if it's been "deleted"/flagged before you render it
  expanded_indices.clear();
  for(int i = 0; i < this->indices.size(); i++) {
    if(indices[i].second) {
      count++;
      aiFace face = this->indices[i].first;
      for(GLuint j = 0; j < face.mNumIndices; j++)
        this->expanded_indices.push_back(face.mIndices[j]);
    }
  }
  
  // Create buffers/arrays
  glGenVertexArrays(1, &this->VAO);
  glGenBuffers(1, &this->VBO);
  glGenBuffers(1, &this->EBO);

  glBindVertexArray(this->VAO);

  // Load data into vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

  // A great thing about structs is that their memory layout is sequential for all its items.
  // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
  // again translates to 3/2 floats which translates to a byte array.
  glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->expanded_indices.size() * sizeof(GLuint), &this->expanded_indices[0], GL_STATIC_DRAW);

  // Set the vertex attribute pointers
  // Vertex Positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
  
  // Vertex Normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
  
  // Old Vertex Positions
  // Load data into vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, this->old_VBO);
  glBufferData(GL_ARRAY_BUFFER, this->old_vertices.size() * sizeof(Vertex), &this->old_vertices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

  glBindVertexArray(0);
  
}

void Mesh::setupQs() {

  for(int v = 0; v < vertices.size(); ++v) {

    Vertex current_vertex = vertices[v];

    // Calculate the matrix Q
    glm::mat4 Q = glm::mat4(0.0f);
    for(int i = 0; i < adj_list[v].size(); ++i) {
      aiFace face = adj_list[v][i].second;
      Vertex v1 = vertices[face.mIndices[0]];
      Vertex v2 = vertices[face.mIndices[1]];
      Vertex v3 = vertices[face.mIndices[2]];

      glm::vec4 p = plane_eq(v1, v2, v3);
      glm::mat4 k = glm::outerProduct(p, p);
      Q += k;
    }

    glm::vec4 temp = glm::vec4(current_vertex.Position, 1.0f);
    float result = glm::dot(temp * Q, temp);

    vertices[v].Q = Q;
    vertices[v].Error = result;
  }

}

void Mesh::setupEdges() {

  for(int i = 0; i < indices.size(); ++i) {
    aiFace face = indices[i].first;
    if(indices[i].second) {
      for(int v = 0; v < face.mNumIndices; ++v) {
        
        // Get the
        unsigned int v1_i = face.mIndices[v];
        unsigned int v2_i = face.mIndices[(v + 1) % face.mNumIndices];
        Vertex v1 = vertices[v1_i];
        Vertex v2 = vertices[v2_i];
        
        bool found = false;
        auto it_1 = edge_check.find(v1_i);
        auto it_2 = edge_check.find(v2_i);
        if(it_1 != edge_check.end()) {
          for(int i = 0; i < it_1->second.size(); ++i) {
            if(it_1->second[i] == v2_i) {
              found = true;
            }
          }
        }
        if(it_2 != edge_check.end()) {
          for(int i = 0; i < it_2->second.size(); ++i) {
            if(it_2->second[i] == v1_i) {
              found = true;
            }
          }
        }
        
        bool is_deleted = v1.deleted || v2.deleted;
        if(!found && !is_deleted) {
          edge_check[v1_i].push_back(v2_i);
          edge_check[v2_i].push_back(v1_i);
          
          Edge new_edge;
          new_edge.v1 = v1;
          new_edge.v2 = v2;
          new_edge.endpoints = make_pair(v1_i, v2_i);
          
          new_edge.Error = v1.Error + v2.Error;
          
          if(v1_i != v2_i) edges.insert(new_edge);
        }
      }
    }
  }

}

void Mesh::MeshCollapse() {
  
  lerp = 0.0f;
  old_vertices = vertices;

  if(!edges.empty()) {
    Edge current = *(edges.begin());
    EdgeCollapse(current.endpoints.first, current.endpoints.second);
    
    int update_id = rand();
    edges.erase(edges.begin());
    edge_stack.push(make_pair(update_id, current));
    UpdateEdges(current, update_id);
  }

}

void Mesh::UpdateEdges(Edge collapsed, int update_id) {

  set<Edge, Compare> temp_e; // Need this to make sure we're iterating through all of the original edges

  for(auto it = edges.begin(); it != edges.end();) {
    Edge e = *it;
    if(collapsed.endpoints.first == it->endpoints.first || collapsed.endpoints.first == it->endpoints.second) { // Recalculate
      it = edges.erase(it); // Need to subtract the iterator to make sure we don't skip an edge
      
      // Calculate perceived error from contraction
      glm::vec4 mid = glm::vec4((e.v1.Position + e.v2.Position) / 2.0f, 1.0f);
      e.Error = glm::dot(mid * (e.v1.Q + e.v2.Q), mid);
      //e.Error = e.v1.Error + e.v2.Error;
      
      //if(temp_e.count(e) == 1) edge_stack.push(make_pair(update_id, e));
      temp_e.insert(e);
    }
    else if(collapsed.endpoints.second == it->endpoints.first || collapsed.endpoints.second == it->endpoints.second) {
      it = edges.erase(it); // Need to subtract the iterator to make sure we don't skip an edge
      
      if(collapsed.endpoints.second == e.endpoints.first) {
        e.v1 = collapsed.v1;
        e.endpoints = make_pair(collapsed.endpoints.first, e.endpoints.second);
      }
      else if(collapsed.endpoints.second == e.endpoints.second) {
        e.v2 = collapsed.v1;
        e.endpoints = make_pair(e.endpoints.first, collapsed.endpoints.first);
      }

      // Calculate perceived error from contraction
      glm::vec4 mid = glm::vec4((e.v1.Position + e.v2.Position) / 2.0f, 1.0f);
      e.Error = glm::dot(mid * (e.v1.Q + e.v2.Q), mid);
      //e.Error = e.v1.Error + e.v2.Error;
      
      //if(temp_e.count(e) == 1) edge_stack.push(make_pair(update_id, e));
      temp_e.insert(e);
    } else {
      it++;
    }
  }

  for(auto it = temp_e.begin(); it != temp_e.end(); it++) {
    edges.insert(*it);
  }
}

void Mesh::EdgeCollapse(GLint v1_i, GLint v2_i) {
  
  // error, cannot decimate (vertices not found)
  if(!adj_list.count(v1_i) || !adj_list.count(v2_i)) return;

  // Get the referenced vertices from out vertex list
  Vertex v1 = vertices[v1_i];
  Vertex v2 = vertices[v2_i];
  
  set<GLint> deleted_face_indices;
  vector<pair<GLint, GLint>> adj_list_indices;
  set<pair<GLint, GLint>> reanchor;
  vector<pair<int,aiFace>> old_v_to_face;

  // Calculate the midpoint to collapse to
  Vertex mid;
  mid.Position = (v1.Position + v2.Position) / 2.0f;
  mid.Normal = glm::normalize((v1.Normal + v2.Normal) / 2.0f);
  mid.Q = v1.Q + v2.Q;

  // Calculate the new error
  glm::vec4 temp = glm::vec4(mid.Position, 1.0f);
  float new_error = glm::dot(temp * mid.Q, temp);
  mid.Error = new_error;

  // Update v1 to use the new midpoint. This automatically updates all faces
  // using v1 to the mid. A bit more work is needed to connect v2's faces.
  // Also mark the second vertex for deletion. It doesn't actually get deleted;
  // this is just a more efficient way to ignore it.
  vertices[v1_i] = mid;
  vertices[v2_i] = mid;

  // Add the faces from v2 to v1 (now the midpoint) in adj list
  old_v_to_face = adj_list[v1_i];
  adj_list[v1_i].insert(adj_list[v1_i].end(), adj_list[v2_i].begin(), adj_list[v2_i].end());

  // Mark faces for deletion
  for(int i = 0; i < adj_list[v1_i].size(); ++i) {
    aiFace face = adj_list[v1_i][i].second;
    int face_i = adj_list[v1_i][i].first;
    int match = 0;

    // Check if two of the vertices are the original v1 / v2. If so, delete this face.
    // Collapsed faces are the faces that share the two original vertices.
    for(int j = 0; j < face.mNumIndices; ++j) {
      if(face.mIndices[j] == v1_i) {
        match++;
      }

      if(face.mIndices[j] == v2_i) {
        anchor.push_back(make_tuple(face, face_i, i, j, v1_i, v2_i));
        face.mIndices[j] = v1_i;
        adj_list[v1_i][i] = make_pair(adj_list[v1_i][i].first, face);
        reanchor.insert(make_pair(face_i, j));
        match++;
      }
    }

    // Delete the degenerate faces from the adj_list, and flag them in the indices
    if(match >= 2) {
      
      if(indices[face_i].second) deleted_face_indices.insert(face_i);
      
      delete_faces_after_lerp.push_back(face_i);
      for(int x = 0; x < adj_list.size(); ++x) {               // TODO: Better way to do this? Literally need to go through all faces
        for(int j = 0; j < adj_list[x].size(); ++j) {
          if(this->adj_list[x][j].second == face) {            // delete face in adj_list if address matches
            this->adj_list[x].erase(adj_list[x].begin() + j);  // remove from vector, this shifts vector left by 1
            if(j == i) i--;                                    // make sure we dont skip a face on accident
            adj_list_indices.push_back(make_pair(x, face_i)); // Note: Not sure if I should reinsert at original position or not...
          }
        }
      }
      num_faces--;
    }

    match = 0;
  }


  //adj_list.erase(v2_i); // Don't need v2 in adj_list anymore
  
  // Need this for progressive meshes
  collapse_stack.push(make_tuple(v1, v2, v1_i, v2_i, deleted_face_indices, adj_list_indices, reanchor, old_v_to_face));
  
}

void Mesh::VertexSplit(){
  
  lerp = 0.0f;
  old_vertices = vertices;
  
  if(collapse_stack.empty()) {
    return;
  }
  
  // Get the last recorded edge collapse
  auto tup = collapse_stack.top();
  collapse_stack.pop();
  
  Vertex v1 = get<0>(tup);
  Vertex v2 = get<1>(tup);
  GLint v1_i = get<2>(tup);
  GLint v2_i = get<3>(tup);
  set<GLint> deleted_face_indices = get<4>(tup);
  vector<pair<GLint, GLint>> adj_list_indices = get<5>(tup);
  set<pair<GLint, GLint>> reanchor = get<6>(tup);
  auto old_v_to_face = get<7>(tup);
  
  cout << "Splitting " << v1_i << " " << v2_i << endl;
  
  // Set vertices back to original locations
  vertices[v1_i] = v1;
  vertices[v2_i] = v2;
  vertices[v2_i].deleted = false;
  
  // We need to "reanchor" the points because we might've switched v2 with v1 in edge collapse
  for(auto it = reanchor.begin(); it != reanchor.end(); ++it)  {
    int face_index = it->first;
    int v_index = it->second;
    
    aiFace face = indices[face_index].first;
    
    face.mIndices[v_index] = v2_i; // Todo: still something weird about this. it assigns to 4, 4.
    
    indices[face_index].first = face;
    //indices[face_index].second = true;
  }
  
  
  // Add deleted faces back in indices, also add back original vertices (arguably most important part)
  for(auto it = deleted_face_indices.begin(); it!= deleted_face_indices.end(); ++it) {
    aiFace face = indices[*it].first;
    indices[*it].second = true;
    
  }
  
  // Add faces back to adjacency list
  for(int i = 0; i < adj_list_indices.size(); ++i) {
    pair<GLint, GLint> current = adj_list_indices[i];
    aiFace face = indices[current.second].first;
    int face_index = current.second;
    int adj_index = current.first;
    adj_list[adj_index].push_back(make_pair(face_index, face));
  }
  
  adj_list[v1_i] = old_v_to_face;
  
  // Add all of the original edges back in edge list
  // TOdo: Should have an edge stack too!
  if(!edge_stack.empty()) {
    int current_id = edge_stack.top().first;
    while(!edge_stack.empty() && edge_stack.top().first == current_id) {
      Edge add_back = edge_stack.top().second;
      edges.insert(add_back);
      edge_stack.pop();
    }
  } else {
    cerr << "Cannot collapse! No more edges in the mesh." << endl;
  }
  
}

glm::vec4 Mesh::plane_eq(Vertex p1, Vertex p2, Vertex p3) {
  glm::vec3 vector_1 = p3.Position - p1.Position;
  glm::vec3 vector_2 = p2.Position - p1.Position;

  glm::vec3 surface_normal = glm::normalize(glm::cross(vector_1, vector_2));
  float d = glm::dot(p1.Position, surface_normal);

  glm::vec4 res = glm::vec4(-surface_normal, d);

  return res;
}
