/*
 * PowerUp.cpp
 *
 *  Created on: 2011-10-14
 *      Author: samuel
 */

#include <fstream>
#include "Avatar.h"
#include "PowerUp.h"

using Game::SpriteLayer::PowerUp;

Game::Audio PowerUp::pickup_sound ("pickup.ogg");

PowerUp::PowerUp (const std::string &subtype) throw(): Sprite (subtype) {
  common = new Common();
  common->reference_count = 1;

  common->animation = new Animation ("sprites/" + subtype + "/animation.png");
  destination_rectangle->w = common->animation->get_width();
  destination_rectangle->h = common->animation->get_height();

  // Load the power up's properties.
  std::string data_filename = "sprites/" + subtype + "/general.dat";
  std::ifstream f (data_filename.c_str());
  f >> common->health_increase;
  f >> common->maximum_health_increase;
  f >> common->energy_increase;
  f.close();
}

PowerUp::PowerUp (const PowerUp &power_up) throw(): Sprite (power_up) {
  common = power_up.common;
  ++common->reference_count;
}

PowerUp::~PowerUp() throw() {
  // Delete the common data once its no longer used by other power-ups.
  if (--common->reference_count == 0) {
    delete common->animation;
    delete common;
  }
}

void PowerUp::update (World *world, unsigned int tick) throw() {
  common->animation->update (tick);
}

void PowerUp::affect (World &world, Sprite &sprite) throw() {
  if (Avatar *avatar = dynamic_cast<Avatar*> (&sprite)) {
    if (!avatar->is_dead()) {
      // Affect the avatar that is touching this power-up.
      avatar->affect_health (common->health_increase);
      avatar->affect_full_health (common->maximum_health_increase);
      avatar->affect_energy (common->energy_increase);

      pickup_sound.play();
      kill (world);
    }
  }
}

void PowerUp::draw (Lighting *lighting) throw() {
  common->animation->draw();
}
