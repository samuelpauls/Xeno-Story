/*
 * Gate.cpp
 *
 *  Created on: 2011-10-14
 *      Author: samuel
 */

#include <fstream>
#include "Projectile.h"
#include "Gate.h"

using Game::SpriteLayer::Gate;

Game::Audio Gate::unbreakable_sound ("gate/unbreakable.ogg");
Game::Audio Gate::open_sound ("gate/open.ogg");

Gate::Gate (const std::string &subtype) throw(): Sprite (subtype) {
  common = new Common();
  common->reference_count = 1;

  // Load the gate's closed animation.
  common->closed_animation =
      new Animation ("sprites/" + subtype + "/closed.png");
  destination_rectangle->w = common->closed_animation->get_width();
  destination_rectangle->h = common->closed_animation->get_height();

  // Load the gate's properties.
  std::string data_filename = "sprites/" + subtype + "/general.dat";
  std::ifstream f (data_filename.c_str());
  std::getline (f, common->weakness);
  f.close();
}

Gate::Gate (const Gate &gate) throw(): Sprite (gate) {
  common = gate.common;
  ++common->reference_count;
}

Gate::~Gate() throw() {
  // Delete the common data once its no longer used by other gates.
  if (--common->reference_count == 0) {
    delete common->closed_animation;
    delete common;
  }
}

void Gate::update (World *world, unsigned int tick) throw() {
  common->closed_animation->update (tick);
}

void Gate::affect (World &world, Sprite &sprite) throw() {
  if (Projectile *projectile = dynamic_cast<Projectile*> (&sprite)) {
    if (projectile->get_subtype() == common->weakness) {
      open_sound.play();
      kill (world);
    } else {
      unbreakable_sound.play();
    }
  }
}

void Gate::draw (Lighting *lighting) throw() {
  common->closed_animation->draw();
}
