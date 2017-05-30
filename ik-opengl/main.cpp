// Std. Includes
#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "ModelObject.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

// Properties
GLuint screenWidth = 800, screenHeight = 600;

// Function prototypes
void key_callback   (GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void Do_Movement(ModelObject * modelObject);

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

bool morphing = true;
float lerp = 0.0;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int decimate_count = 0;

// The MAIN function, from here we start our application and run our Game loop
int main()
{
  
  // Init GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Model Viewer", nullptr, nullptr);
  glfwMakeContextCurrent(window);

  // Set the required callback functions
  glfwSetKeyCallback      (window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetScrollCallback   (window, scroll_callback);

  // Initialize GLEW to setup the OpenGL Function pointers
  glewExperimental = GL_TRUE;
  glewInit();

  // Define the viewport dimensions
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);

  // Setup some OpenGL options
  glEnable(GL_DEPTH_TEST);

  // Game loop
  while(!glfwWindowShouldClose(window))
  {
    // Set frame time
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Check and call events
    glfwPollEvents();

    // Clear the colorbuffer
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Transformation matrices
    glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    
    // TODO: Render Chain here
    
    // Swap the buffers
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}

#pragma region "User input"

// Moves/alters the camera positions based on user input
void Do_Movement(ModelObject * modelObject)
{
  // Camera controls
  if(keys[GLFW_KEY_W])
    camera.ProcessTranslation(FORWARD, deltaTime *.8);
  if(keys[GLFW_KEY_S])
    camera.ProcessTranslation(BACKWARD, deltaTime *.8);
  if(keys[GLFW_KEY_A])
    camera.ProcessTranslation(LEFT, deltaTime * .8);
  if(keys[GLFW_KEY_D])
    camera.ProcessTranslation(RIGHT, deltaTime * .8);

  if(keys[GLFW_KEY_UP])
    modelObject->ProcessRotation(FORWARD, deltaTime);
  if(keys[GLFW_KEY_DOWN])
    modelObject->ProcessRotation(BACKWARD, deltaTime);
  if(keys[GLFW_KEY_LEFT])
    modelObject->ProcessRotation(LEFT, deltaTime);
  if(keys[GLFW_KEY_RIGHT])
    modelObject->ProcessRotation(RIGHT, deltaTime);

  if(keys[GLFW_KEY_O]) {
    modelObject->decimate();
    decimate_count++;
  }
  
  if(keys[GLFW_KEY_P]) {
    modelObject->progress();
  }
  
  if(keys[GLFW_KEY_K]) {
    if(modelObject->step > 1) modelObject->step -= 10;
    else modelObject->step = 1;
    
    cout << "Step is now " << modelObject->step << endl;
    keys[GLFW_KEY_K] = false;
  }
  
  if(keys[GLFW_KEY_L]) {
    if(modelObject->step < 100) modelObject->step += 10;
    else modelObject->step = 100;
    cout << "Step is now " << modelObject->step << endl;
    keys[GLFW_KEY_L] = false;
  }
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);

  if(action == GLFW_PRESS)
    keys[key] = true;
  else if(action == GLFW_RELEASE)
    keys[key] = false;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
     fprintf(stderr, "Click\n");
  }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(yoffset);
}

#pragma endregion
