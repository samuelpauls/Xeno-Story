/*
 * Projectile.cpp
 *
 *  Created on: 2011-08-20
 *      Author: samuel
 */

#include <cmath>
#include <fstream>
#include "../Scripting/Double.h"
#include "../Scripting/Integer.h"
#include "Avatar.h"
#include "Projectile.h"

using Game::SpriteLayer::Projectile;

void Projectile::register_properties() throw() {
  object.consume_variable (new Scripting::Double ("Angle Degrees", angle_degrees));
  object.consume_variable (new Scripting::Integer ("Team", team));
}

void Projectile::on_script_variable_changed (const std::string &name) throw() {
  if (name == "Angle Degrees") {
    set_angle (angle_degrees); // set the radian angle
  }
}

Projectile::Projectile (const std::string &subtype) throw():
    SpriteLayer::Sprite(subtype) {
  common = new Common;
  common->reference_count = 1;

  flying_animation = new Animation ("sprites/" + subtype + "/flying.png");
  destination_rectangle->w = flying_animation->get_width();
  destination_rectangle->h = flying_animation->get_height();
  current_animation = flying_animation;

  // If there's a hit animation, load it.
  std::string hit_filename = "sprites/" + subtype + "/hit.png";
  std::ifstream hit_file (hit_filename.c_str());
  if (hit_file) { // If the hit animation file exists...
    hit_file.close();
    hit_animation = new Animation (hit_filename);
  } else {
    hit_animation = NULL;
  }

  start_tick = NOT_SET;

  angle_degrees = 0.0;
  angle_radians = 0.0;

  std::string data_filename = "sprites/" + subtype + "/general.dat";
  std::ifstream f (data_filename.c_str());
  f >> common->rotate_animations;
  f >> common->speed; // in pixels per second
  f >> common->total_dist; // in pixels
  f >> common->light.radius;
  f >> common->light.red >>
       common->light.green >>
       common->light.blue; // [0, 255]

  f.close();

  register_properties();
}

Projectile::Projectile (const Projectile &projectile) throw():
    Sprite (projectile) {
  common = projectile.common;
  ++common->reference_count;

  start_tick = projectile.start_tick;
  start_x = projectile.start_x;
  start_y = projectile.start_y;
  angle_degrees = projectile.angle_degrees;
  angle_radians = projectile.angle_radians;
  team = projectile.team;
  flying_animation = new Animation (*projectile.flying_animation);
  if (projectile.hit_animation) {
    hit_animation = new Animation (*projectile.hit_animation);
  } else {
    hit_animation = NULL;
  }
  if (projectile.current_animation == projectile.flying_animation) {
    current_animation = flying_animation;
  } else if (projectile.current_animation == projectile.hit_animation) {
    current_animation = hit_animation;
  }

  register_properties();
}

Projectile::~Projectile() throw() {
  delete flying_animation;
  delete hit_animation;

  // Delete the common data once no other cloned projectiles are using it.
  if (--common->reference_count == 0) {
    delete common;
  }
}

void Projectile::set_position (int x, int y) throw() {
  Sprite::set_position (x, y);
  start_x = x;
  start_y = y;
}

void Projectile::set_angle (double angle) throw() {
  angle_degrees = angle;

  static const double PI = 3.141592;
  angle_radians = angle_degrees / 180.0 * PI;
}

void Projectile::set_team (int team) throw() {
  this->team = team;
}

void Projectile::update (World *world, unsigned int tick) throw() {
  if (world) {
    int dist;

    if (current_animation == flying_animation) {
      // Move the projectile.
      if (start_tick == NOT_SET) {
        flying_animation->rewind (tick);
        start_tick = tick;
      }
      dist = (tick - start_tick) * common->speed / 1000;
      int dist_x = std::cos (angle_radians) * dist; // cos * hyp = adj
      int dist_y = std::sin (angle_radians) * dist; // sin * hyp = opp
      destination_rectangle->x = start_x + dist_x;
      destination_rectangle->y = start_y - dist_y;

      // Switch to the hit animation upon travelling the total distance or
      // hitting the environment.
      if (dist >= common->total_dist ||
          world->is_colliding_with_environment (*this)) {
        if (hit_animation) {
          hit_animation->rewind (tick);
        }
        current_animation = hit_animation;
      }
    } else if (current_animation == hit_animation) {
      // Destroy the projectile if it hit something and the hit animation, if
      // present, has finished playing.
      if (!hit_animation || hit_animation->get_times_played() >= 1.0F) {
        kill (*world);
      }
    } // if animation

    // Destroy any damageable blocks this projectile may be touching.
    world->hit_tiles (*destination_rectangle);
  }
  if (current_animation) {
    current_animation->update (tick);
  }
}

void Projectile::affect (World &world, Sprite &sprite) throw() {
  if (Avatar *avatar = dynamic_cast<Avatar*> (&sprite)) {
    if (avatar->get_team() != team) {
      avatar->affect_health (-1);
      kill (world);
    }
  }
}

void Projectile::draw (Lighting *lighting) throw() {
  // Draw the projectile.
  if (current_animation) {
    glPushMatrix();
    if (angle_degrees) {
      glTranslatef (current_animation->get_width() / 2,
                    current_animation->get_height() / 2,
                    0.0F);
      glRotatef (-angle_degrees, 0.0F, 0.0F, 1.0F);
      glTranslatef (-current_animation->get_width() / 2,
                    -current_animation->get_height() / 2,
                    0.0F);
    }
    current_animation->draw();
    glPopMatrix();
  }

  // Draw the light effects.
  if (lighting) {
    glColor3f (common->light.red, common->light.green, common->light.blue);
    glPushMatrix();
    glTranslatef (destination_rectangle->w / 2,
                  destination_rectangle->h / 2,
                  0.0F);
    lighting->add_light (common->light.radius);
    glPopMatrix();
  }
}
