//
//  Target.cpp
//

#include "Target.h"
#include "Camera.h"

Target::Target() {
  
  // Create the shader to use for the controller
  Shader modelS(vertexShaderPath, fragShaderPath);
  objectShader = modelS;
  
  // Creates the model for the controller
  Model modelM (pathToModel);
  objectModel = modelM;
  
  // Sets the position / rotation / scale
  position = glm::vec3(0, 0, 0);
  scale = glm::vec3 (.1f, .1f, .1f);
  pitch = 0.0f;
  yaw = 0.0f;
}

void Target::Render(glm::mat4 view, glm::mat4 proj) {
  
  objectShader.Use();
  
  GLint objectColorLoc = glGetUniformLocation(objectShader.Program, "objectColor");
  GLint lightColorLoc = glGetUniformLocation(objectShader.Program, "lightColor");
  GLint lightPosLoc = glGetUniformLocation(objectShader.Program, "lightPos");
  GLint viewPosLoc = glGetUniformLocation(objectShader.Program, "viewPos");
  
  glUniform3f(objectColorLoc, 1.0f, 1.0f,1.0f);
  glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
  glUniform3f(lightPosLoc, 1.0f ,1.0f, 3.0f);
  glUniform3f(viewPosLoc,0.0, 0.0, 3.0);
  
  // Calculate the toWorld matrix for the model
  glm::mat4 model;
  glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
  glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
  glm::mat4 R = glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(1, 0, 0));
  R = glm::rotate(R, yaw, glm::vec3(0, 0, 1));
  model = T * S * R;
  
  glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
  
  objectModel.Draw(objectShader);

}

void Target::ProcessTranslation(Camera_Movement direction, GLfloat deltaTime) {
  GLfloat velocity = 3.0f * deltaTime;
  if (direction == UP)
    this->position.y += 1.0f * velocity;
  if (direction == DOWN)
    this->position.y -= 1.0f * velocity;
  if (direction == LEFT)
    this->position.x -= 1.0f * velocity;
  if (direction == RIGHT)
    this->position.x += 1.0f * velocity;
  if (direction == FORWARD)
    this->position.z += 1.0f * velocity;
  if (direction == BACKWARD)
    this->position.z -= 1.0f * velocity;
  
}
