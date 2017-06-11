//
//  MultiChain.hpp
//  ik-opengl
//
//  Created by Jesse Zhou on 6/9/17.
//  Copyright © 2017 Jesse and Robb. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <vector>
#include <stack>
#include <map>

#include "Chain.h"

// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/projection.hpp>
#include <GL/glew.h>

// Tree data structure
struct ChainNode {
  Chain * value;
  ChainNode * parent;
  vector<ChainNode*> * children;
};

class MultiChain {

public:
  MultiChain(vector<Chain*> chains);
  bool Insert(ChainNode * root, Chain * chain);
  void Solve();
  void Render(glm::mat4 view, glm::mat4 proj);

  ChainNode * root;
  map<ChainNode*, bool> leaves;
  glm::vec3 origin;
  
private:
  void Forward(ChainNode * root);
  void Backward(ChainNode * root);
};
