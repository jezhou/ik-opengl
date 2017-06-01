//
//  Object.hpp
//  ik-opengl
//
//  Created by Jesse Zhou on 6/1/17.
//  Copyright © 2017 Jesse and Robb. All rights reserved.
//

#pragma once

#include <stdio.h>

#include "Model.h"
#include "Shader.h"

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

class Object {
public:
  Object();
  virtual void Render(glm::mat4 view, glm::mat4 proj);
  virtual void init();
  
protected:
  Model model;
  GLchar* model_path;
  GLchar* vs_path;
  GLchar* fs_path;
  Shader shader;
  
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;
};
