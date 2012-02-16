/*
 * SavePoint.cpp
 *
 *  Created on: 2011-10-21
 *      Author: samuel
 */

#include <fstream>
#include "Avatar.h"
#include "SavePoint.h"

using Game::SpriteLayer::SavePoint;

SavePoint::SavePoint (const std::string &subtype) throw(): Sprite (subtype) {
  common = new Common();
  common->reference_count = 1;

  // Load the idle animation.
  common->idle = new Animation ("sprites/" + subtype + "/idle.png");
  destination_rectangle->w = common->idle->get_width();
  destination_rectangle->h = common->idle->get_height();
  current_animation = common->idle;

  // Load the activated animation if it exists.
  std::string activated_filename = "sprites/" + subtype + "/activated.png";
  std::ifstream activated_file (activated_filename.c_str());
  if (activated_file) {
    activated_file.close();
    common->activated = new Animation (activated_filename);
  } else {
    common->activated = NULL;
  }
}

SavePoint::SavePoint (const SavePoint &save_point) throw():
    Sprite (save_point) {
  common = save_point.common;
  ++common->reference_count;

  current_animation = save_point.current_animation;
}

SavePoint::~SavePoint() throw() {
  // Delete the shared data once all clones are done with it.
  if (--common->reference_count == 0) {
    delete common->idle;
    delete common->activated;
    delete common;
  }
}

void SavePoint::update (World *world, unsigned int tick) throw() {
  current_animation->update (tick);
}

void SavePoint::affect (World &world, Sprite &sprite) throw() {
  if (&sprite == world.get_player()) {
    if (world.get_player()->get_controller()->is_down_pressed()) {
      if (common->activated) {
        current_animation = common->activated;
      }
      world.save();
    }
  }
}

void SavePoint::draw (Lighting *lighting) throw() {
  current_animation->draw();

  // Add a light if activated.
  if (current_animation == common->activated) {
    glTranslatef (current_animation->get_width() / 2.0F, 3.0F, 0.0F);
    glColor3ub (156, 120, 252); // gun metal purple
    lighting->add_light (40);
  }
}
