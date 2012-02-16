/*
 * Elevator.cpp
 *
 *  Created on: 2011-12-12
 *      Author: samuel
 */

#include <cassert>
#include <algorithm> // for std::max
#include <cmath>
#include "../Scripting/Integer.h"
#include "Elevator.h"

using Game::SpriteLayer::Elevator;

void Elevator::register_properties() throw() {
  object.consume_variable (new Scripting::Integer ("X End", x_end));
  object.consume_variable (new Scripting::Integer ("Y End", y_end));
}

void Elevator::on_script_variable_changed (const std::string &name) throw() {
  if (name == "X" || name == "Y") {
    path.remove_node (0);
    path.insert_node (0, destination_rectangle->x, destination_rectangle->y,
                      common->speed_per_second);
  } else if (name == "X End" || name == "Y End") {
    path.remove_node (1);
    path.push_node (x_end, y_end, common->speed_per_second);
  }
  path_follower.set_path (&path);
}

Elevator::Elevator (const std::string &subtype) throw():
    SpriteLayer::Sprite (subtype) {
  common = new Common();
  common->reference_count = 1;

  common->animation = new Animation ("sprites/" + subtype + "/animation.png");
  destination_rectangle->w = common->animation->get_width();
  destination_rectangle->h = common->animation->get_height();

  common->speed_per_second = 10;

  path.push_node (destination_rectangle->x, destination_rectangle->y,
                  common->speed_per_second);
  path.push_node (destination_rectangle->x, destination_rectangle->y,
                  common->speed_per_second);
  path_follower.set_path (&path);

  register_properties();
}

Elevator::Elevator (const Elevator &elevator) throw():
                   Sprite (elevator), path (elevator.path),
                   path_follower (elevator.path_follower) {
  common = elevator.common;
  ++common->reference_count;

  register_properties();
}

Elevator::~Elevator() throw() {
  if (--common->reference_count == 0) {
    delete common->animation;
    delete common;
  }
}

void Elevator::update (World *world, unsigned int tick) throw() {
  common->animation->update (tick);

  // If the elevator is in the world...
  if (world) {
    // Update the elevator's location and note the change in it's location.
    path_follower.update (tick);
    int dx = path_follower.get_x() - destination_rectangle->x;
    int dy = path_follower.get_y() - destination_rectangle->y;
    destination_rectangle->x = path_follower.get_x();
    destination_rectangle->y = path_follower.get_y();

    // If the elevator moved...
    if (dx || dy) {
      // Get the sprites standing on the elevator.
      Rectangle rect;
      rect.x = destination_rectangle->x;
      rect.y = destination_rectangle->get_top() + 1;
      rect.w = destination_rectangle->w;
      rect.h = 1;
      std::list<Sprite*> sprites = world->get_touching_sprites (rect);

      // Move those sprites.
      for (std::list<Sprite*>::iterator s = sprites.begin();
           s != sprites.end();
           ++s) {
        Rectangle spr_pos = (*s)->get_position();
        spr_pos.x += dx;
        spr_pos.y += dy;
        (*s)->set_position (spr_pos.x, spr_pos.y);
      } // for
    } // if elevator moved
  }
}

void Elevator::draw (Lighting *lighting) throw() {
  common->animation->draw();
}
