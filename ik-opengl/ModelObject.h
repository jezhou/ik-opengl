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

class ModelObject {
public:
  glm::vec3 position;
  float pitch;
  float yaw;
  glm::vec3 scale;
  glm::quat orientation;
  int step = 1;
  bool polygon_mode = false;

  // Functions
  ModelObject(char * modelName);
  void Render(glm::mat4 view, glm::mat4 proj);
  void ProcessRotation(Camera_Movement direction, GLfloat deltaTime);
  void decimate();
  void progress();
  float getLerp();

private:

  /* Data */
  Model objectModel;
  Shader objectShader;

  /* Functions */


};
