/*
 * Explosion.cpp
 *
 *  Created on: 2011-10-10
 *      Author: samuel
 */

#include <cstdlib> // for std::rand
#include "../Scripting/Integer.h"
#include "Avatar.h"
#include "Explosion.h"

using Game::SpriteLayer::Explosion;

void Explosion::register_properties() throw() {
  object.consume_variable (new Scripting::Integer ("Magnitude", magnitude));
}

Explosion::Explosion (const std::string &subtype) throw(): Sprite (subtype) {
  common = new Common();
  common->animation = new Animation ("sprites/" + subtype + "/animation.png");
  common->sound = new Audio ("sprites/" + subtype + "/sound.ogg");
  common->reference_count = 1;

  magnitude = 2000;
  destination_rectangle->w = 100;
  destination_rectangle->h = 100;

  team = 0;

  previous_tick = 0;
  next_part_tick = magnitude;
  spawn_wait = 100;

  register_properties();
}

Explosion::Explosion (const Explosion &explosion) throw(): Sprite (explosion) {
  common = explosion.common;
  common->reference_count++;

  magnitude = explosion.magnitude;

  team = explosion.team;

  previous_tick = explosion.previous_tick;
  next_part_tick = explosion.next_part_tick;
  spawn_wait = explosion.spawn_wait;

  register_properties();
}

Explosion::~Explosion() throw() {
  // Delete the explosion's parts.
  for (std::list<Animation*>::iterator p = parts.begin();
       p != parts.end();
       ++p) {
    delete *p;
  }

  // If the common data is no longer used by other explosions, delete it.
  if (--common->reference_count == 0) {
    delete common->animation;
    delete common->sound;
    delete common;
  }
}

void Explosion::update (World *world, unsigned int tick) throw() {
  if (magnitude > 0) {
    // Decrease the explosion's magnitude.
    if (previous_tick) {
      magnitude -= tick - previous_tick;
    }
    previous_tick = tick;

    if (magnitude <= next_part_tick) {
      // Create a new part for the overall explosion.
      Animation *part_animation = new Animation (*common->animation);
      part_animation->rewind (tick);
      part_animation->set_draw_offset_x (((float)std::rand() / RAND_MAX) *
          (destination_rectangle->w - part_animation->get_height()));
      part_animation->set_draw_offset_y (((float)std::rand() / RAND_MAX) *
          (destination_rectangle->h - part_animation->get_height()));
      parts.push_back (part_animation);

      common->sound->play();
      world->quake (magnitude / 80);

      next_part_tick += spawn_wait;
      spawn_wait += 200;
    }
  }

  // Update the explosion's parts.
  for (std::list<Animation*>::iterator p = parts.begin();
       p != parts.end();
       ++p) {
    Animation *anim = *p;
    if (anim->is_done()) {
      delete anim;
      p = parts.erase (p);
    } else {
      anim->update (tick);
    }
  }

  // End the explosion once all parts have fired.
  if (magnitude <= 0 && parts.empty()) {
    kill (*world);
  }
}

void Explosion::affect (World &world, Sprite &sprite) throw() {
  if (Avatar *avatar = dynamic_cast<Avatar*> (&sprite)) {
    if (avatar->get_team() != team) {
      avatar->affect_health (-1);
    }
  }
}

void Explosion::draw (Lighting *lighting) throw() {
  for (std::list<Animation*>::const_iterator p = parts.begin();
       p != parts.end();
       ++p) {
    Animation &anim = **p;
    anim.draw();
    float intensity = 1.0F - anim.get_times_played(); // 1 -> 0
    if (intensity > 0.0) {
      glColor3f (intensity, intensity * 0.5F, intensity * 0.5F);
      glPushMatrix();
      glTranslatef (anim.get_width() / 2, anim.get_height() / 2, 0.0F);
      static const int RADIUS = 50;
      lighting->add_light (RADIUS);
      glPopMatrix();
    }
  }
}
