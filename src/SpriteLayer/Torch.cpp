/*
 * Torch.cpp
 *
 *  Created on: 2011-09-11
 *      Author: samuel
 */

#include <typeinfo> // has typeid
#include "../Scripting/Boolean.h"
#include "Projectile.h"
#include "Torch.h"

using Game::SpriteLayer::Torch;

void Torch::register_properties() throw() {
  object.consume_variable (new Scripting::Boolean ("Lit", lit));
}

Torch::Torch (const std::string &subtype) throw(): SpriteLayer::Sprite(subtype) {
  out_animation = new Animation ("sprites/" + subtype + "/out.png");
  lit_animation = new Animation ("sprites/" + subtype + "/lit.png");
  destination_rectangle->w = out_animation->get_width();
  destination_rectangle->h = out_animation->get_height();
  lit = false;
  previous_tick = 0;
  ticks_since_flicker = 0;
  light_radius = 100;
  register_properties();
}

Torch::Torch (const Torch &torch) throw(): Sprite (torch) {
  out_animation = new Animation (*torch.out_animation);
  lit_animation = new Animation (*torch.lit_animation);
  lit = torch.lit;
  previous_tick = torch.previous_tick;
  ticks_since_flicker = torch.ticks_since_flicker;
  light_radius = torch.light_radius;
  register_properties();
}

Torch::~Torch() throw() {
  delete out_animation;
  delete lit_animation;
}

void Torch::update (World *world, unsigned int tick) throw() {
  if (lit) {
    lit_animation->update (tick);
  } else {
    out_animation->update (tick);
  }
  if (previous_tick != 0) {
    ticks_since_flicker += tick - previous_tick;
  }
  previous_tick = tick;

  // Update the light radius flickering.
  static const int FLICKER_WAIT = 100; // in ms
  if (ticks_since_flicker >= FLICKER_WAIT) {
    if (light_radius == 100) {
      light_radius = 95;
    } else {
      light_radius = 100;
    }
    ticks_since_flicker -= FLICKER_WAIT;
  }
}

void Torch::affect (World &world, Sprite &sprite) throw() {
  if (typeid (sprite) == typeid (Projectile)) {
    if (!lit) {
      lit = true;

      // The torch consumes the projectile when being lit.
      sprite.kill (world);
    }
  }
}

void Torch::draw (Lighting *lighting) throw() {
  // Draw the torch.
  if (lit) {
    lit_animation->draw();
  } else {
    out_animation->draw();
  }

  // Draw the lighting effects.
  if (lighting) {
    if (lit) {
      glColor3f (1.0F, 0.8F, 0.2F);
      glPushMatrix();
      glTranslatef (destination_rectangle->w / 2,
                    destination_rectangle->h / 2,
                    0.0F);
      lighting->add_light (light_radius);
      glPopMatrix();
    }
  }
}
