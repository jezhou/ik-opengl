//
//  Segment.cpp
//  ik-opengl
//
//  Created by Jesse Zhou on 6/1/17.
//  Copyright © 2017 Jesse and Robb. All rights reserved.
//

#include "Segment.h"

#include "Target.h"
#include "Camera.h"

Segment::Segment() {
  
  // Create the shader to use for the controller
  Shader modelS(vertexShaderPath, fragShaderPath);
  objectShader = modelS;
  
  // Creates the model for the controller
  Model modelM (pathToModel);
  objectModel = modelM;
  
  // Sets the position / rotation / scale
  position = glm::vec3(0, 0, 0);
  scale = glm::vec3 (.1f, .1f, 1.0f);
  pitch = -45.0f;
  yaw = -45.0f;
}

void Segment::Render(glm::mat4 view, glm::mat4 proj) {
  
  yaw += 0.01f;
  
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
  model = T * R * S;
  
  glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(glGetUniformLocation(objectShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
  
  // Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
  glUniform1f(glGetUniformLocation(objectShader.Program, "material.shininess"), 16.0f);
  
  // VBO stuff
  
  float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, // Bottom face
    0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    
    -0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    
    0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
    
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    0.0f, -0.0f,  0.0f,  0.0f, -1.0f,  0.0f,
    
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    0.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,
  };
  // first, configure the cube's VAO (and VBO)
  unsigned int VBO, cubeVAO;
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &VBO);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  
  glBindVertexArray(cubeVAO);
  
  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  
  // normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  
  
  glDrawArrays(GL_TRIANGLES, 0, 36);
  
}
