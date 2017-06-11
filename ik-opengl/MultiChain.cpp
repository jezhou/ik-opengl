//
//  MultiChain.cpp
//  ik-opengl
//
//  Created by Jesse Zhou on 6/9/17.
//  Copyright © 2017 Jesse and Robb. All rights reserved.
//

#include "MultiChain.h"

MultiChain::MultiChain(vector<Chain*> chains) {
  root = new ChainNode();
  root->value = *chains.begin();
  root->parent = NULL;
  root->children = new vector<ChainNode*>();
  bool success = false;
  
  for(auto it = chains.begin() + 1; it != chains.end(); ++it) {
    Chain * current = *it;
    success = true && Insert(root, current);
  }
  
  if(!success) {
    cerr << "Something went wrong when constructing the multichain; one of the nodes probably didn't match up." << endl;
    exit(-1);
  }
}

bool MultiChain::Insert(ChainNode * root, Chain * chain) {
  if(glm::all(glm::equal(root->value->end, chain->origin))) {
    
    ChainNode * new_node = new ChainNode();
    new_node->value = chain;
    new_node->parent = root;
    new_node->children = new vector<ChainNode*>();
    root->children->push_back(new_node);
    return true;
    
  } else if(root->children->size()) {
    
    bool was_inserted = false;
    for(auto it = root->children->begin(); it != root->children->end(); ++it) {
      ChainNode * recurse = *it;
      was_inserted = was_inserted || Insert(recurse, chain);
      if(was_inserted) break;
    }
    return was_inserted;
    
  }
  
  return false; // Cannot insert
}

void MultiChain::Solve() {
  
  
  
}
