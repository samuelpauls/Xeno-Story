/*
 * Weapon.cpp
 *
 *  Created on: 2011-08-26
 *      Author: samuel
 */

#include <climits> // for INT_MAX
#include <cmath> // for std::sqrt
#include <fstream>
#include "../Scripting/Integer.h"
#include "Projectile.h"
#include "Explosion.h"
#include "Weapon.h"

using Game::SpriteLayer::Weapon;

void Weapon::register_properties() throw() {
  object.consume_variable (new Scripting::Integer ("level", level));
  object.consume_variable (new Scripting::Integer ("energy", energy));
}

Weapon::Weapon (const std::string &subtype) throw(): SpriteLayer::Sprite(subtype) {
  common = new Common();
  common->reference_count = 1;

  // If this weapon has an animation, load it.
  std::string graphic_filename = "sprites/" + subtype + "/animation.png";
  std::ifstream f_exists (const_cast<const char*> (graphic_filename.c_str()));
  if (f_exists) {
    f_exists.close();
    common->animation = new Animation (graphic_filename);
    destination_rectangle->w = common->animation->get_width();
    destination_rectangle->h = common->animation->get_height();
  } else {
    // Some weapons don't have graphics.  After all, they may only be available
    // to certain kinds of sprites when spawning and not as pickups.
    common->animation = NULL;
  }

  // If there's a muzzle flash animation, load it.
  std::string flash_filename = "sprites/" + subtype + "/flash.png";
  std::ifstream flash_file (flash_filename.c_str());
  if (flash_file) { // If the hit animation file exists...
    flash_file.close();
    common->flash = new Animation (flash_filename);
  } else {
    common->flash = NULL;
  }

  fire_tick = 0;
  shots_fired = INT_MAX;
  firing_avatar = NULL;

  std::string config_filename = "sprites/" + subtype + "/general.dat";
  std::ifstream f (const_cast<const char*>(config_filename.c_str()));

  f >> common->fire_delay; // in ms

  // Read the weapon's level data.
  while (!f.eof()) {
    std::string line_remainder;
    std::getline (f, line_remainder);

    Common::Level level;

    // Load the sprite that will be shot.
    std::string shot_subtype;
    std::getline (f, shot_subtype);
    level.shot_sprite = create (shot_subtype);

    f >> level.energy_to_next_level;

    std::getline (f, line_remainder);

    std::getline (f, level.fire_command);

    // Load the level's shot data.
    level.range = 0;
    int projectile_count;
    f >> projectile_count;
    for (int p = 0; p < projectile_count; ++p) {
      Common::Level::Shot shot;
      f >> shot.delay >> shot.offset_x >> shot.offset_y >> shot.angle;
      level.shots.push_back (shot);

      // If this offset from the fire origin is the farthest, remember it.
      int shot_offset = std::sqrt (shot.offset_x * shot.offset_x + shot.offset_y * shot.offset_y);
      if (shot_offset > level.range) {
        level.range = shot_offset;
      }
    } // for each shot in the level

    // Add on the maximum distance of the projectile, if applicable.
    if (Projectile *projectile =
        dynamic_cast<Projectile*> (level.shot_sprite)) {
      level.range += projectile->get_maximum_distance();
    }

    common->levels.push_back (level);
  } // while there are levels of fire commands

  f.close();

  // If there's a fire sound for this weapon, load it.
  std::string fire_sound_filename = "sprites/" + subtype + "/fire.ogg";
  std::ifstream fire_sound_file (fire_sound_filename.c_str());
  if (fire_sound_file) {
    fire_sound_file.close();
    common->fire_sound = new Audio (fire_sound_filename);
  } else {
    common->fire_sound = NULL;
  }

  level = 1;
  energy = 0;

  flash_rect = NULL;

  register_properties();
}

Weapon::Weapon (const Weapon &weapon) throw(): Sprite (weapon) {
  common = weapon.common;
  common->reference_count++;

  flash_rect = weapon.flash_rect;
  shots_fired = weapon.shots_fired;
  firing_avatar = weapon.firing_avatar;
  level = weapon.level;
  energy = weapon.energy;
  fire_tick = weapon.fire_tick;

  register_properties();
}

Weapon::~Weapon() throw() {
  delete flash_rect;

  // Delete the common data once no more clones of this subtype are using it.
  if (--common->reference_count == 0) {
    // Delete the weapon's levels.
    for (std::vector<Common::Level>::iterator l = common->levels.begin();
         l != common->levels.end();
         ++l) {
      delete l->shot_sprite;
    }

    delete common->animation;
    delete common->flash;
    delete common->fire_sound;
    delete common;
  }
}

void Weapon::affect_energy (int change) throw() {
  energy += change;

  if (energy < 0) {
    if (level == 1) {
      // Limit energy loss to no energy at the first level.
      energy = 0;
    } else {
      // Level down.
      --level;
      energy += common->levels[level - 1].energy_to_next_level;
    }
  } else if (energy > 0) {
    // Level up if there's enough energy.
    while (level < (int)common->levels.size() &&
           energy >= common->levels[level - 1].energy_to_next_level) {
      energy -= common->levels[level - 1].energy_to_next_level;
      ++level;
    }

    // Limit energy at the final level.
    if (level == (int)common->levels.size() &&
        energy > common->levels[level - 1].energy_to_next_level) {
      energy = common->levels[level - 1].energy_to_next_level;
    }
  }
}

void Weapon::fire (World &world, const Avatar &avatar,
                   unsigned int tick) throw() {
  if (tick >= fire_tick + common->fire_delay) {
    // Start a new muzzle flash animation if the animation exists.
    if (common->flash) {
      if (!flash_rect) {
        flash_rect = new Rectangle();
      }
      flash_rect->x = avatar.get_shot_origin_x();
      flash_rect->y = avatar.get_shot_origin_y();
      common->flash->rewind (tick);
    } // if muzzle flash animation

    world.queue_command (common->levels[level - 1].fire_command);

    shots_fired = 0;
    firing_avatar = &avatar;

    fire_tick = tick;
  } // weapon delay
}

void Weapon::update (World *world, unsigned int tick) throw() {
  if (common->animation) {
    common->animation->update (tick);
  }

  // Update the muzzle flash.
  if (flash_rect) {
    common->flash->update (tick);
    if (common->flash->get_times_played() >= 1.0F) {
      delete flash_rect;
      flash_rect = NULL;
    }
  }

  // Fire shots from this weapon at appropriate times.
  while (shots_fired < (int)common->levels[level - 1].shots.size() &&
         tick - fire_tick >=
             common->levels[level - 1].shots[shots_fired].delay) {
    Sprite *shot_sprite = common->levels[level - 1].shot_sprite->clone();
    Projectile *shot_projectile = dynamic_cast<Projectile*> (shot_sprite);
    Explosion *shot_explosion = dynamic_cast<Explosion*> (shot_sprite);

    // Set the shot sprite's position.
    Rectangle avatar_pos = firing_avatar->get_position();
    int shot_offset_x = common->levels[level - 1].shots[shots_fired].offset_x;
    int shot_offset_y = common->levels[level - 1].shots[shots_fired].offset_y;
    shot_sprite->set_position (
        avatar_pos.x + firing_avatar->get_shot_origin_x() + shot_offset_x,
        avatar_pos.y + firing_avatar->get_shot_origin_y() + shot_offset_y);

    // Set the shot's team if applicable.
    if (shot_projectile) {
      shot_projectile->set_team (firing_avatar->get_team());
    } else if (shot_explosion) {
      shot_explosion->set_team (firing_avatar->get_team());
    }

    // Set the projectile's angle.
    if (shot_projectile) {
      int angle = common->levels[level - 1].shots[shots_fired].angle;
      if (firing_avatar->get_direction() == Avatar::LEFT) {
        angle = -180 - angle;
      }
      shot_projectile->set_angle (angle);
    }

    world->add (*shot_sprite);
    if (common->fire_sound) {
      common->fire_sound->play();
    }
    ++shots_fired;
  } // for each shot that it's time to fire
}

void Weapon::affect (World &world, Sprite &sprite) throw() {
  Avatar *player = world.get_player();
  if (&sprite == player) {
    world.detatch_sprite (*this);
    player->pickup_weapon (*this);
  }
}

void Weapon::draw (Lighting *lighting) throw() {
  if (common->animation) {
    common->animation->draw();
  }
}

void Weapon::draw_flash (Lighting *lighting) throw() {
  if (flash_rect) {
    glTranslatef (flash_rect->x, flash_rect->y, 0.0F);
    common->flash->draw();
  }
}
