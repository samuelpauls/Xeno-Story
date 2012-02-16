/*
 * Path.cpp
 *
 *  Created on: 2012-02-04
 *      Author: samuel
 */

#include <cassert>
#include "Path.h"

using Game::SpriteLayer::Path;

Path::Path() {}

Path::Path (const Path &path) throw() {
  for (unsigned int n = 0; n < path.nodes.size(); ++n) {
    nodes.push_back (new Node (*path.nodes[n]));
  }
}

Path::~Path() {
  // Delete the nodes.
  for (unsigned int n = 0; n < nodes.size(); ++n) {
    delete nodes[n];
  }
}

void Path::push_node (int x, int y, unsigned int speed_per_second) throw() {
  Node *node = new Node();
  node->x = x;
  node->y = y;
  node->speed_per_second = speed_per_second;
  nodes.push_back (node);
}

void Path::insert_node (int index,
                        int x, int y,
                        unsigned int speed_per_second) throw() {
  assert (index < (int)nodes.size());
  Node *node = new Node();
  node->x = x;
  node->y = y;
  node->speed_per_second = speed_per_second;
  nodes.insert (nodes.begin() + index, node);
}

void Path::remove_node (int index) throw() {
  assert (index < (int)nodes.size());
  delete nodes[index];
  nodes.erase (nodes.begin() + index);
}
