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

Chain::Chain(glm::vec3 origin, glm::vec3 end, Target * t, int partitions) {
  
  vector<float> lengths;
  vector<glm::quat> directions;
  vector<glm::vec3> joints;
  
  glm::vec3 partition_checkpoint = origin;
  glm::vec3 dir = glm::normalize(end - origin);
  float magnitude = glm::length(end - origin);
  float partial_mag = magnitude / (float)partitions;
  
  joints.push_back(origin);
  for(int i = 1; i <= partitions; ++i) {
    glm::vec3 to_insert = partition_checkpoint + partial_mag * i * dir;
    joints.push_back(to_insert);
  }
  
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
    for(int i = 0; i < joints.size() - 1; ++i) {
      float r = glm::length(target->position - joints[i]);
      float l = segments[i].magnitude / r;
      joints[i + 1] = (1 - l) * joints[i] + l * target->position;
    }
    
    vector<float> lengths;
    vector<glm::quat> directions;
    CalculateLinks(joints, &lengths, &directions);
  
  } else { // Target is in reach, use FABRIK to solve both back and forwards
    int count = 0;
    float difference = glm::length(joints[joints.size() - 1] - target->position);
    while(difference > tolerance) {
      Backward();
      Forward();
      difference = glm::length(joints[joints.size() - 1] - target->position);
      count++;
      
      if(count > 10) break;
    }

  }
  
  SetSegments();
  
}

void Chain::SetSegments() {
  vector<float> lengths;
  vector<glm::quat> directions;
  CalculateLinks(joints, &lengths, &directions);
  
  for(int i = 0; i < lengths.size(); ++i) {
    segments[i].Set(joints[i], lengths[i], directions[i]);
  }
}

void Chain::Backward() {
  auto end = joints.end() - 1;
  *end = target->position;
  for(int i = int(joints.size() - 2); i >= 0; --i) {
    float r = glm::length(joints[i+1] - joints[i]);
    
    float l = segments[i].magnitude / r;
    joints[i] = (1 - l) * joints[i+1] + l * joints[i];
  }
}

void Chain::Forward() {
  auto beg = joints.begin();
  *beg = origin;
  for(int i = 0; i < joints.size() - 1; ++i) {
    float r = glm::length(joints[i+1] - joints[i]);
    
    float l = segments[i].magnitude / r;
    joints[i + 1] = (1 - l) * joints[i] + l * joints[i+1];
  }
}

void Chain::CalculateLinks(vector<glm::vec3> joints, vector<float> * lengths, vector<glm::quat> * directions) {
  
  origin = *joints.begin();
  end = *(joints.end() - 1);
  
  for(auto it = joints.begin(); it != joints.end() - 1; ++it) {
    
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

glm::vec3 Chain::GetFirstJoint() {
  return joints[0];
}

void Chain::SetFirstJoint(glm::vec3 joint) {
  joints[0] = joint;
}
