//
//  Chain.cpp
//  ik-opengl
//
//  Created by Jesse Zhou on 6/2/17.
//  Copyright © 2017 Jesse and Robb. All rights reserved.
//

#include "Chain.h"

Chain::Chain(vector<glm::vec3> joints, Target * t) {
  
  vector<float> lengths;
  vector<glm::quat> directions;
  CalculateLinks(joints, &lengths, &directions);
  
  for(int i = 0; i < lengths.size(); ++i) {
    segments.push_back(Segment(joints[i], lengths[i], directions[i]));
    total_length += lengths[i];
  }
  
  target = t;
  size = joints.size();
  this->joints = joints;
  
}

void Chain::Solve() {
  
  float current_distance = glm::length(target->position - origin);
  
  // Target is out of reach; fully extend the arm
  if (current_distance > total_length) {
    cout << "Target out of reach! Elongating chain..." << endl;
    for(int i = 0; i < joints.size() - 1; ++i) {
      float r = glm::length(target->position - joints[i]);
      float l = segments[i].magnitude / r;
      joints[i + 1] = (1 - l) * joints[i] + l * target->position;
    }
    
    vector<float> lengths;
    vector<glm::quat> directions;
    CalculateLinks(joints, &lengths, &directions);
    
    for(int i = 0; i < lengths.size(); ++i) {
      segments[i].Set(joints[i], lengths[i], directions[i]);
    }
  }
}

void Chain::CalculateLinks(vector<glm::vec3> joints, vector<float> * lengths, vector<glm::quat> * directions) {
  for(auto it = joints.begin(); it != joints.end() - 1; ++it) {
    
    if(it == joints.begin()) origin = *it;
    
    glm::vec3 current = *it;
    glm::vec3 next = *(it + 1);
    
    glm::vec3 link_vector = next - current;
    
    // Get magnitude of link
    lengths->push_back(glm::length(link_vector));
    
    // Get rotation of link
    glm::vec3 cross = glm::cross(ref_rot_vector, link_vector);
    glm::quat q;
    q.x = cross.x;
    q.y = cross.y;
    q.z = cross.z;
    q.w = sqrt(pow(glm::length(link_vector), 2) * pow(glm::length(ref_rot_vector), 2)) + glm::dot(link_vector, ref_rot_vector);
    directions->push_back(glm::normalize(q));
  }
}

void Chain::Render(glm::mat4 view, glm::mat4 proj) {
  for(auto it = segments.begin(); it != segments.end(); ++it) {
    it->Render(view, proj);
  }
}
