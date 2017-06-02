#pragma once

// Std. Includes
#include <vector>

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  UP,
  DOWN
};

// Default camera values
const GLfloat YAW        =  -90.0f;
const GLfloat PITCH      =  0.0f;
const GLfloat SPEED      =  3.0f;
const GLfloat SENSITIVTY =  0.25f;
const GLfloat ZOOM       =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
  
  // Camera Attributes
  glm::vec3 Position;
  glm::vec3 Front;
  glm::vec3 Up;
  glm::vec3 Right;
  glm::vec3 WorldUp;
  
  // Eular Angles
  GLfloat Yaw;
  GLfloat Pitch;
  
  // Camera options
  GLfloat MovementSpeed;
  GLfloat MouseSensitivity;
  GLfloat Zoom;
  
  // Constructor with vectors
  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
         GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
  {
    this->Position = position;
    this->WorldUp = up;
    this->Yaw = yaw;
    this->Pitch = pitch;
    this->updateCameraVectors();
  }
  // Constructor with scalar values
  Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
  {
    this->Position = glm::vec3(posX, posY, posZ);
    this->WorldUp = glm::vec3(upX, upY, upZ);
    this->Yaw = yaw;
    this->Pitch = pitch;
    this->updateCameraVectors();
  }
  
  // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
  glm::mat4 GetViewMatrix()
  {
    return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
  }
  
  // Processes input received from the keyboard
  void ProcessTranslation(Camera_Movement direction, GLfloat deltaTime)
  {
    GLfloat velocity = this->MovementSpeed * deltaTime;
    if (direction == FORWARD)
      this->Position -= glm::vec3(0, 1.0f, 0) * velocity;
    if (direction == BACKWARD)
      this->Position += glm::vec3(0, 1.0f, 0) * velocity;
    if (direction == LEFT)
      this->Position += this->Right * velocity;
    if (direction == RIGHT)
      this->Position -= this->Right * velocity;
  }
  
  // Processes input received from a mouse scroll-wheel event
  void ProcessMouseScroll(GLfloat yoffset)
  {
    float zoomSmallLimit = 47.0f;
    float zoomLargeLimit = 44.5f;
   
    yoffset *= .1f;
    if (this->Zoom >= zoomLargeLimit && this->Zoom <= zoomSmallLimit)
      this->Zoom -= yoffset;
    if (this->Zoom <= zoomLargeLimit)
      this->Zoom = zoomLargeLimit;
    if (this->Zoom >= zoomSmallLimit)
      this->Zoom = zoomSmallLimit;
  }
  
private:
  // Calculates the front vector from the Camera's (updated) Eular Angles
  void updateCameraVectors()
  {
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
    front.y = sin(glm::radians(this->Pitch));
    front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
    this->Front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
  }
};
