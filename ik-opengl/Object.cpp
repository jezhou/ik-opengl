//
//  Object.cpp
//  ik-opengl
//
//  Created by Jesse Zhou on 6/1/17.
//  Copyright © 2017 Jesse and Robb. All rights reserved.
//

#include "Object.hpp"

Object::Object() {
  
  init(); // Initialize all of the shader / model paths, as well as position, scale, etc.
          // Init needs to be defined in a child class.
  
  shader = Shader(vs_path, fs_path);
  model = Model(model_path);
}
