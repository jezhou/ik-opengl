//
//  ModelObject.cpp
//

#include "ModelObject.h"
#include "Camera.h"
#include <unistd.h>


ModelObject::ModelObject(char * modelName) {

  // Create the shader to use for the controller
  char pathToModel[1000];
  char vertexShaderPath[1000];
  char fragShaderPath[1000];
  getcwd(vertexShaderPath, PATH_MAX);
  strcat(vertexShaderPath, "/Models/shader.vs");
  getcwd(fragShaderPath, PATH_MAX);
  strcat(fragShaderPath, "/Models/shader.frag");
  
  Shader modelS(vertexShaderPath, fragShaderPath);
  objectShader = modelS;

  // Creates the model for the controller
  getcwd(pathToModel, PATH_MAX);
  strcat(pathToModel, "/Models/");
  strcat(pathToModel, modelName);
  objectModel = Model(pathToModel);

  // Sets the position / rotation / scale
  position = glm::vec3(0, 0, 0);
  scale = glm::vec3 (.1f, .1f, .1f);
  pitch = 0.0f;
  yaw = 0.0f;
}

void ModelObject::Render(glm::mat4 view, glm::mat4 proj) {

  objectShader.Use();

  GLint objectColorLoc = glGetUniformLocation(objectShader.Program, "objectColor");
  GLint lightColorLoc = glGetUniformLocation(objectShader.Program, "lightColor");
  GLint lightPosLoc = glGetUniformLocation(objectShader.Program, "lightPos");
  GLint viewPosLoc = glGetUniformLocation(objectShader.Program, "viewPos");

  glUniform3f(objectColorLoc, 1.0f, 1.0f,1.0f);
  glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
  glUniform3f(lightPosLoc, 1.0f ,1.0f, 1.0f);
  glUniform3f(viewPosLoc,0.0, 0.0, 0.0);

  // Calculate the toWorld matrix for the model
  glm::mat4 model;
  glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
  glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);

  glm::quat p_quat = glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0));
  glm::quat y_quat = glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0));

  glm::quat quat = p_quat * y_quat;
  orientation = quat * orientation; // Note: Order matters!! This rotates around a world axis (orientation * quat rotates around a local axis)

  model = T * S * glm::toMat4(orientation);

  glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
  glUniform1f(glGetUniformLocation(objectShader.Program, "lerp"), objectModel.meshes[0].lerp);
  //glUniform1f(glGetUniformLocation(objectShader.Program, "lerp"), 5.0);


  objectModel.Draw(objectShader);
  if(polygon_mode) glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE ) ;
  yaw = 0.0f;
  pitch = 0.0f;

}

void ModelObject::ProcessRotation(Camera_Movement direction, GLfloat deltaTime) {
  GLfloat velocity = 3.0f * deltaTime;
  if (direction == FORWARD)
    this->pitch = -50.0f * velocity;
  if (direction == BACKWARD)
    this->pitch = 50.0f * velocity;
  if (direction == LEFT)
    this->yaw = -50.0f * velocity;
  if (direction == RIGHT)
    this->yaw = 50.0f * velocity;

}

void ModelObject::decimate() {
  if(objectModel.meshes[0].lerp >= 1.0f) { // DO NOT DISTURB LERPING
    for(int i = 0; i < step; ++i)
    {
      objectModel.meshes[0].MeshCollapse();
    }
    objectModel.meshes[0].setupMesh();
  }
}

void ModelObject::progress() {
  if(objectModel.meshes[0].lerp >= 1.0f) { // DO NOT DISTRUB LERPING
    for(int i = 0; i < step; ++i) {
      objectModel.meshes[0].VertexSplit();
    }
    objectModel.meshes[0].setupMesh();
  }
}
