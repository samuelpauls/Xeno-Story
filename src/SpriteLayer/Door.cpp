/*
 * Door.cpp
 *
 *  Created on: 2011-10-25
 *      Author: samuel
 */

#include <cassert>
#include "../Scripting/String.h"
#include "../Scripting/Boolean.h"
#include "Avatar.h"
#include "Door.h"

using Game::SpriteLayer::Door;

bool Door::can_use_doors = true;

void Door::register_properties() throw() {
  object.consume_variable (new Scripting::String ("Destination Map Name", destination_map_name));
  object.consume_variable (new Scripting::String ("Destination Door ID", destination_door_id));
  object.consume_variable (new Scripting::Boolean ("Open", open));
}

Door::Door (const std::string &subtype) throw(): SpriteLayer::Sprite (subtype) {
  common = new Common();
  common->reference_count = 1;

  // Load the open animation.
  common->open_animation = new Animation ("sprites/" + subtype + "/open.png");
  destination_rectangle->w = common->open_animation->get_width();
  destination_rectangle->h = common->open_animation->get_height();

  common->closed_animation =
      new Animation ("sprites/" + subtype + "/closed.png");

  open = false;

  register_properties();
}

Door::Door (const Door &door) throw(): Sprite (door) {
  common = door.common;
  ++common->reference_count;

  open = door.open;
  destination_map_name = door.destination_map_name;
  destination_door_id = door.destination_door_id;

  register_properties();
}

Door::~Door() throw() {
  // Delete the shared data once all clones are done with it.
  if (--common->reference_count == 0) {
    delete common->open_animation;
    delete common->closed_animation;
    delete common;
  }
}

void Door::update (World *world, unsigned int tick) throw() {
  if (open) {
    common->open_animation->update (tick);
  } else {
    common->closed_animation->update (tick);
  }
}

void Door::affect (World &world, Sprite &sprite) throw() {
  if (world.get_player() &&
      world.get_player()->get_controller()->is_down_pressed()) {
    if (can_use_doors) {
      if (open) {
        if (&sprite == world.get_player()) {
          world.teleport_player (destination_map_name, destination_door_id);
          can_use_doors = false;
        }
      }
    }
  } else {
    can_use_doors = true;
  }
}

void Door::draw (Lighting *lighting) throw() {
  if (open) {
    common->open_animation->draw();
  } else {
    common->closed_animation->draw();
  }
}
