//
//  Target.hpp
//  ik-opengl
//
//  Created by Jesse Zhou on 6/1/17.
//  Copyright © 2017 Jesse and Robb. All rights reserved.
//

#pragma once

#include <stdio.h>

#include "Object.hpp"

class Target : public Object {
public:
  Target() {};
  void init();
  void Render();
};
