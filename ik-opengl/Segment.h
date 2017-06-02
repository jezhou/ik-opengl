//
//  Segment.hpp
//  ik-opengl
//
//  Created by Jesse Zhou on 6/1/17.
//  Copyright © 2017 Jesse and Robb. All rights reserved.
//


#include <stdio.h>

/////////////////////
// ModelObject.h
/////////////////////

#pragma once
#include "Shader.h"
#include "Model.h"
#include "Camera.h"

// Std includes
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

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

class Segment {
public:
  glm::vec3 position;
  glm::vec3 scale;
  float pitch;
  float yaw;
  
  // Functions
  Segment();
  void Render(glm::mat4 view, glm::mat4 proj);
  void ProcessTranslation(Camera_Movement direction, GLfloat deltaTime);
  
private:
  
  /* Data */
  Model objectModel;
  GLchar* pathToModel      = "/Users/jezhou/Git/classes/s17/ik-opengl/cone.off";
  GLchar* vertexShaderPath = "/Users/jezhou/Git/classes/s17/ik-opengl/seg.vs";
  GLchar* fragShaderPath   = "/Users/jezhou/Git/classes/s17/ik-opengl/seg.frag";
  Shader objectShader;
  
  /* Functions */
  
  
};
