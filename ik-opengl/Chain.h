//
//  Chain.h
//  ik-opengl
//
//  Created by Jesse Zhou on 6/2/17.
//  Copyright © 2017 Jesse and Robb. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <vector>

#include "Target.h"
#include "Segment.h"

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

const glm::vec3 ref_rot_vector(0, 0, -1);

struct Link {
  glm::vec3 joint;
  float length;
  glm::quat dir;
};

class Chain {
public:
  Chain(vector<glm::vec3> joints, Target * t);
  void Render(glm::mat4 view, glm::mat4 proj);
  void Solve();
  void CalculateLinks(vector<glm::vec3> joints, vector<float> * lengths, vector<glm::quat> * directions);
  
  unsigned long size;
  float total_length;
  glm::vec3 origin;
  
private:
  vector<glm::vec3> joints; // Joints themselves
  vector<Segment> segments; // The pieces that actually get rendered
  Target * target;
  
};
