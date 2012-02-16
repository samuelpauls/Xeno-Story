/*
 * PathFollower.cpp
 *
 *  Created on: 2012-02-08
 *      Author: samuel
 */

#include <cstring> // for NULL
#include <cmath>
#include "PathFollower.h"

using Game::SpriteLayer::PathFollower;

int PathFollower::get_previous_node_index (int index) throw() {
  if (index == 0) {
    return path->nodes.size() - 1;
  } else {
    return index - 1;
  }
}

int PathFollower::get_next_node_index (int index) throw() {
  if (index >= (int)path->nodes.size() - 1) {
    return 0;
  } else {
    return index + 1;
  }
}

unsigned int PathFollower::get_ms_to_next() const throw() {
  int dx = path->nodes[next_node_index]->x - start_x;
  int dy = path->nodes[next_node_index]->y - start_y;
  float dist = std::sqrt (dx * dx + dy * dy);
  return dist / path->nodes[current_node_index]->speed_per_second * 1000;
}

PathFollower::PathFollower() throw() {
  path = NULL;
  start_tick = 0;
  x = 0;
  y = 0;
}

PathFollower::PathFollower (const PathFollower &path_follower) throw() {
  path = path_follower.path;

  current_node_index = path_follower.current_node_index;
  next_node_index = path_follower.next_node_index;

  start_tick = path_follower.start_tick;
  start_x = path_follower.start_x;
  start_y = path_follower.start_y;

  x = path_follower.x;
  y = path_follower.y;
}

PathFollower::~PathFollower() throw() {};

void PathFollower::set_position (int x, int y) throw() {
  start_x = x;
  start_y = y;
  this->x = x;
  this->y = y;
  start_tick = 0; // unset
}

void PathFollower::set_path (Path *path) throw() {
  this->path = path;

  current_node_index = 0;
  next_node_index = get_next_node_index (0);

  set_position (path->nodes[0]->x, path->nodes[0]->y);
}

void PathFollower::update (unsigned int tick) throw() {
  if (start_tick) {
    unsigned int elapsed_ms = tick - start_tick; // frm start of cur node
    unsigned int ms_to_next = get_ms_to_next();

    // Wrap to the first node
    while (elapsed_ms >= ms_to_next) {
      elapsed_ms -= ms_to_next;
      start_tick = tick;
      current_node_index = next_node_index;
      start_x = path->nodes[current_node_index]->x;
      start_y = path->nodes[current_node_index]->y;
      next_node_index = get_next_node_index (next_node_index);
      ms_to_next = get_ms_to_next();
      if (ms_to_next == 0) {
        break;
      }
    }

    // Update the current point.
    double progress;
    if (ms_to_next) {
      progress = ((double)elapsed_ms) / ms_to_next;
    } else {
      progress = 0.0;
    }
    x = start_x * (1.0 - progress)
        + path->nodes[next_node_index]->x * progress;
    y = start_y * (1.0 - progress)
        + path->nodes[next_node_index]->y * progress;
  } else {
    start_tick = tick;
  }
}
