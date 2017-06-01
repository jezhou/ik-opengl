//
//  Chain.hpp
//  ik-opengl
//
//  Created by Jesse Zhou on 5/31/17.
//  Copyright © 2017 Jesse and Robb. All rights reserved.
//

#pragma once

#include <stdio.h>

// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <GL/glew.h>

#include <vector>

using namespace glm;
using namespace std;

class Chain {

public:
  
  Chain();
  
  void solve();
  
private:
  
  vector<vec3> joints;
  vector<float> lengths;
  vec3 origin;
  float tolerance;
  float total_length;
  
};
