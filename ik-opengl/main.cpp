// Std. Includes
#include <string>
#include <iostream>
#include <cstring>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Target.h"
#include "Chain.h"
#include "MultiChain.h"
#include "Leap.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Properties
GLuint screenWidth = 800, screenHeight = 600;

// Function prototypes
void key_callback   (GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void Do_Movement(Target * target);
void ProcessFrame(const Leap::Controller & controller, Target * target);

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// The MAIN function, from here we start our application and run our Game loop
int main()
{
  
  char desired_model[1000];
  cout << "IK OpenGL - Jesse & Robb - CSE 163, Project 4, SP17\n1: Single chain\n2: Multichain\n3: Single Chain w/ Constraint\nEnter the model you want here: ";
  cin >> desired_model;
  
  cout << desired_model << endl;
  
  // Init GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_SAMPLES, 4);
  
  GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Fast and Backwards Reaching Inverse Kinematic (FABRIK) Solver - Robb & Jesse - CSE 163", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  
  // Set the required callback functions
  glfwSetKeyCallback      (window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  
  // Initialize GLEW to setup the OpenGL Function pointers
  glewExperimental = GL_TRUE;
  glewInit();
  
  // Define the viewport dimensions
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  
  // Setup some OpenGL options
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  // Load joints
  vector<glm::vec3> joints1;
  for(int i = 0; i < 10; ++i) {
    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    joints1.push_back(glm::vec3(0, r, 0));
  }
  
  vector<glm::vec3> joints2;
  joints2.push_back(glm::vec3(0, 0.0f, 0));
  joints2.push_back(glm::vec3(0, 1.0f, 0));
  joints2.push_back(glm::vec3(1.0f, 2.0f, 0));
  
  // Load our model object
  Target target(1.0f, 2.0f, 0);
  Target target2(2, 0, 0);
  Target target3(1, 1, 0);
  Chain chain1(joints1, &target);
  Chain chain2(joints2, &target);
  chain2.please_constrain = true;
  
  vector<Chain*> vec;
  vec.push_back(new Chain(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), &target));
  vec.push_back(new Chain(glm::vec3(0, 1, 0), glm::vec3(1, 1, 0), &target, 2));
  vec.push_back(new Chain(glm::vec3(0, 1, 0), glm::vec3(-1, 2, 0), &target3, 2));
  vec.push_back(new Chain(glm::vec3(0, 1, 0), glm::vec3(-1, 1.5, 0), &target2, 2));
  MultiChain multichain(vec);
  
  // Leap motion stuff
  Leap::Controller controller;
  bool controller_msg_displayed = false;
  
  // Game loop
  while(!glfwWindowShouldClose(window))
  {
    if(controller.isConnected() && !controller_msg_displayed) {
      cout << "Leap Motion is connected." << endl;
      controller_msg_displayed = true;
    }
    
      
    // Set frame time
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    
    // Check and call events
    glfwPollEvents();
    
    // Clear the colorbuffer
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    Do_Movement(&target);
    
    if(controller.isConnected()) ProcessFrame(controller, &target);
    
    // Transformation matrices
    glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
    
    glm::mat4 view = camera.GetViewMatrix();
    target.Render(view, projection);
    
    if(strcmp(desired_model, "1") == 0) {
      chain1.Solve();
      chain1.Render(view, projection);
    } else if(strcmp(desired_model, "2") == 0) {
      multichain.Solve();
      multichain.Render(view, projection);
      target2.Render(view, projection);
      target3.Render(view, projection);
    } else if(strcmp(desired_model, "3") == 0) {
      chain2.Solve();
      chain2.Render(view, projection);
    } else {
      cout << "Invalid chain model" << endl;
      break;
    }
    
    // Swap the buffers
    glfwSwapBuffers(window);
  }
  
  glfwTerminate();
  return 0;
}

#pragma region "User input"

// Moves/alters the target position based on user input
void Do_Movement(Target * target)
{
  
  if(keys[GLFW_KEY_LEFT_SHIFT] && keys[GLFW_KEY_UP])
    target->ProcessTranslation(FORWARD, deltaTime);
  else if(keys[GLFW_KEY_UP])
    target->ProcessTranslation(UP, deltaTime);
  
  if(keys[GLFW_KEY_LEFT_SHIFT] && keys[GLFW_KEY_DOWN])
    target->ProcessTranslation(BACKWARD, deltaTime);
  else if(keys[GLFW_KEY_DOWN])
    target->ProcessTranslation(DOWN, deltaTime);
  
  if(keys[GLFW_KEY_LEFT])
    target->ProcessTranslation(LEFT, deltaTime);
  if(keys[GLFW_KEY_RIGHT])
    target->ProcessTranslation(RIGHT, deltaTime);
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

#pragma endregion

// Leap motion stuff

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};

void ProcessFrame(const Leap::Controller & controller, Target * target) {
  // Get the most recent frame and report some basic information
  const Leap::Frame frame = controller.frame();
  glm::vec3 pos = target->position;
  
  // Right now, this just gets the tip of the right index finger and tracks its position.
  Leap::HandList hands = frame.hands();
  for (Leap::HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
    const Leap::Hand hand = *hl;
    if(hand.isRight()) {
      Leap::FingerList fl = hand.fingers();
      for(auto it = fl.begin(); it != fl.end(); ++it) {
        Leap::Finger f = *it;
        if(f.type() == Leap::Finger::TYPE_INDEX) {
          Leap::Vector leap_pos = f.tipPosition();
          pos = glm::vec3(leap_pos.x, leap_pos.y, leap_pos.z);
          pos /= 40.0f;
          pos = pos - glm::vec3(0, 5.0f, 0);
          break;
        }
      }
      break;
    }
    
  }
  
  target->position = pos;

}

