/*
 * Avatar.cpp
 *
 *  Created on: 2011-09-03
 *      Author: samuel
 */

#include <cstdlib> // for std::malloc
#include <cassert>
#include <fstream>
#include <typeinfo>
#include <GL/gl.h>
#include "../StringTokenizer.h"
#include "../Scripting/String.h"
#include "../Scripting/Integer.h"
#include "../Scripting/Boolean.h"
#include "../Controllers/AI.h"
#include "Weapon.h"
#include "Avatar.h"

using Game::SpriteLayer::Avatar;

Game::Audio Avatar::switch_weapon_sound ("switch_weapon.ogg");

const std::string Avatar::STATE_NAMES[] = {"still", "running", "stopping",
                                           "jumping", "falling"};

int Avatar::dist_from_falling_with_gravity (int milliseconds) throw() {
  float t = milliseconds / 1000.0; // in seconds

  static const float G = 9.8;
  float d = 0.5 * G * t * t; // in meters

  static const float WORLD_MULTIPLIER = 30.0;
  return d * WORLD_MULTIPLIER;
}

void Avatar::set_size (int width, int height) throw() {
  destination_rectangle->w = width;
  destination_rectangle->h = height;

  common->explosion->set_size (width, height);

  // Position the left facing weapons and arms that hold them.
  for (int wc = 0; wc < common->weapon_container_count; ++wc) {
    Animation *whal = weapon_containers[wc].holding_animations[LEFT];
    Animation *whar = weapon_containers[wc].holding_animations[RIGHT];
    if (whal != NULL && whar != NULL) {
      whal->set_draw_offset_x (destination_rectangle->w -
          whar->get_draw_offset_x() - whal->get_width());
    }
  }
}

void Avatar::register_properties() throw() {
  object.consume_variable (new Scripting::Integer ("Health", health));
  object.consume_variable (new Scripting::String ("Death Command", death_command));
  object.consume_variable (new Scripting::Boolean ("God", god));

  for (int wc = 0; wc < common->weapon_container_count; ++wc) {
    WeaponContainer &weapon_container = weapon_containers[wc];
    std::string var_name = "Has " + weapon_container.name;
    object.consume_variable (new Scripting::Boolean (var_name, weapon_container.equipped));
  }
}

Avatar::Avatar (const std::string &subtype) throw(): SpriteLayer::Sprite(subtype) {
  common = new Common();
  common->reference_count = 1;

  std::string config_filename = "sprites/" + subtype + "/general.dat";
  std::ifstream f (config_filename.c_str());

  // Setup health.
  f >> common->full_health;
  health = common->full_health;
  recently_hurt = false;
  hurt_tick = NOT_HURT;
  god = false;

  // Setup movement.
  f >> common->speed_per_second;
  f >> common->jump_duration; // in ms
  f >> common->weight;
  direction = RIGHT;
  set_state (STILL);
  moving_horizontally = false;
  blocked = false;

  f >> common->team;

  // Load the death explosion.
  std::string explosion_type;
  std::getline (f, explosion_type); // required to finish off the last line
  std::getline (f, explosion_type); // read the explosion type for real
  if (explosion_type == "none") {
    common->explosion = NULL;
  } else {
    common->explosion =
        dynamic_cast<Explosion*> (Sprite::create (explosion_type));
    assert (common->explosion);
    common->explosion->set_magnitude (get_full_health() * 100);
  }

  // Setup weapon carrying.
  f >> common->weapon_container_count;
  weapon_index = 0;
  weapon_mode = UNARMED;
  std::string line;
  std::getline (f, line); // required to finish off the line
  weapon_containers = new WeaponContainer[common->weapon_container_count];
  for (int wc = 0; wc < common->weapon_container_count; ++wc) {
    WeaponContainer &weapon_container = weapon_containers[wc];

    std::getline (f, line);
    StringTokenizer tokens (line);
    weapon_container.name = tokens.get_next_string();
    weapon_container.equipped = false;
    weapon_container.shot_offset_x = tokens.get_next_integer();
    weapon_container.shot_offset_y = tokens.get_next_integer();
    weapon_container.weapon = NULL;
    if (tokens.has_more_tokens()) {
      if (tokens.get_next_string() == "equipped") {
        Weapon *weapon = dynamic_cast<Weapon*> (
            Sprite::create (weapon_container.name));
        assert (weapon);
        pickup_weapon (*weapon);
      }
    }

    // Setup weapon holding animations.
    std::string filename = "sprites/" + subtype + "/" +
        "weapons" + "/" + weapon_container.name + ".png";
    for (int d = 0; d < DIRECTION_COUNT; ++d) {
      std::ifstream f (filename.c_str());
      if (f) {
        f.close();
        weapon_container.holding_animations[d] = new Animation (filename);
      } else {
        weapon_container.holding_animations[d] = NULL;
      }
    }
    Animation *whal = weapon_container.holding_animations[LEFT];
    if (whal) {
      whal->flip_horizontally();
    }
  }

  // Load the pickups this sprite drops upon death.
  while (!f.eof()) {
    std::string subtype;
    std::getline (f, subtype);
    PowerUp *power_up = dynamic_cast<PowerUp*> (create (subtype));
    assert (power_up);
    common->pickups.push_back (power_up);
  }

  f.close();

  // If this avatar has a hit sound, load it.
  std::string hit_sound_filename = "sprites/" + subtype + "/hit.ogg";
  std::ifstream hit_sound_file (hit_sound_filename.c_str());
  if (hit_sound_file) {
    hit_sound_file.close();
    common->hit_sound = new Audio (hit_sound_filename);
  } else {
    common->hit_sound = NULL;
  }

  push_controller (new Controllers::Avatar()); // push dummy controller
  push_controller (new Controllers::AI());
  damage_flicker = false;
  register_properties();
}

Avatar::Avatar (const Avatar &avatar) throw(): Sprite (avatar) {
  common = avatar.common;
  common->reference_count++;

  // Movement
  set_state (avatar.state);
  direction = avatar.direction;
  moving_horizontally = avatar.moving_horizontally;
  horizontal_movement_start_x = avatar.horizontal_movement_start_x;
  horizontal_movement_start_tick = avatar.horizontal_movement_start_tick;
  vertical_movement_start_y = avatar.vertical_movement_start_y;
  vertical_movement_start_tick = avatar.vertical_movement_start_tick;
  blocked = avatar.blocked;

  // Weapons
  weapon_mode = avatar.weapon_mode;
  weapon_index = avatar.weapon_index;
  weapon_containers = new WeaponContainer[common->weapon_container_count];
  for (int wc = 0; wc < common->weapon_container_count; ++wc) {
    WeaponContainer &new_wc = weapon_containers[wc];
    WeaponContainer &old_wc = avatar.weapon_containers[wc];

    new_wc.name = old_wc.name;
    new_wc.equipped = old_wc.equipped;
    if (old_wc.weapon) {
      new_wc.weapon = new Weapon(*old_wc.weapon);
      equipped_weapons.push_back (new_wc.weapon);
    } else {
      new_wc.weapon = NULL;
    }
    new_wc.shot_offset_x = old_wc.shot_offset_x;
    new_wc.shot_offset_y = old_wc.shot_offset_y;
    for (int d = 0; d < DIRECTION_COUNT; ++d) {
      if (old_wc.holding_animations[d]) {
        new_wc.holding_animations[d] =
            new Animation (*old_wc.holding_animations[d]);
      } else {
        new_wc.holding_animations[d] = NULL;
      }
    }
  }

  health = avatar.health;
  recently_hurt = avatar.recently_hurt;
  hurt_tick = avatar.hurt_tick;
  damage_flicker = avatar.damage_flicker;
  god = avatar.god;

  // Controllers
  for (std::list<Controllers::Avatar*>::const_iterator c = avatar.controllers.begin();
       c != avatar.controllers.end();
       ++c) {
    controllers.push_back ((*c)->clone());
  }

  register_properties();
}

Avatar::~Avatar() throw() {
  // Delete the weapon containers.
  for (int wc = 0; wc < common->weapon_container_count; ++wc) {
    delete weapon_containers[wc].weapon;
    for (int d = 0; d < DIRECTION_COUNT; ++d) {
      delete weapon_containers[wc].holding_animations[d];
    }
  }
  delete[] weapon_containers;

  // Delete the controllers.
  for (std::list<Controllers::Avatar*>::iterator c = controllers.begin();
       c != controllers.end();
       ++c) {
    delete *c;
  }

  // If no other copied sprites are using the common data, delete it.
  if (--common->reference_count == 0) {
    delete common->hit_sound;
    delete common->explosion;

    // Delete this avatar's death pickups.
    for (std::list<PowerUp*>::iterator p = common->pickups.begin();
         p != common->pickups.end();
         ++p) {
      delete *p;
    }

    delete common;
  }
}

void Avatar::on_script_variable_changed (const std::string &name) throw() {
  if (name.substr (0, 3) == "Has") {
    // Create weapons for this sprite if it's missing something that's always
    // equipped.
    for (int wc = 0; wc < common->weapon_container_count; ++wc) {
      WeaponContainer &weapon_container = weapon_containers[wc];
      if (weapon_container.equipped && !weapon_container.weapon) {
        Weapon *weapon = dynamic_cast<Weapon*> (
            Sprite::create (weapon_container.name));
        assert (weapon);
        pickup_weapon (*weapon);
      } else if (!weapon_container.equipped && weapon_container.weapon) {
        // Unequip the weapon.
        for (std::vector<Weapon*>::iterator w = equipped_weapons.begin();
             w != equipped_weapons.end();
             ++w) {
          if (*w == weapon_container.weapon) {
            equipped_weapons.erase (w);
            break;
          }
        }

        // Remove the weapon from inventory.
        delete weapon_container.weapon;
        weapon_container.weapon = NULL;

        // Equip a new weapon if possible.
        if (equipped_weapons.empty()) {
          weapon_mode = UNARMED;
        } else {
          if (!weapon_containers[weapon_index].weapon) {
            for (int wc = 0; wc < common->weapon_container_count; ++wc) {
              if (weapon_containers[wc].weapon) {
                weapon_index = wc;
                break;
              }
            }
          }
        } // has equipped weapon(s)
      }
    }
  }
}

void Avatar::pop_controller() throw() {
  // Ensure the dummy controller will remain after the pop.
  assert (controllers.size() >= 2);

  controllers.pop_back();
}

void Avatar::set_position (int x, int y) throw() {
  Sprite::set_position (x, y);
  set_state (STILL);
}

void Avatar::affect_full_health (int change) throw() {
  common->full_health += change;
  health += change;
}

void Avatar::affect_health (int change) {
  // If the avatar's health is improving or it was injured without god mode...
  if (change > 0 || !god) {
    if (change > 0 || hurt_tick == NOT_HURT) {
      // Modify the health.
      health += change;
      if (health > get_full_health()) {
        health = get_full_health();
      }

      if (change < 0) {
        recently_hurt = true;
      }
    }
  }

  // If damage was attempted on this avatar and it has a hit sound, play it.
  if (change < 0 && hurt_tick == NOT_HURT) {
    if (common->hit_sound) {
      common->hit_sound->play();
    }
  }
}

void Avatar::affect_energy (int change) throw() {
  if (weapon_mode == ARMED) {
    weapon_containers[weapon_index].weapon->affect_energy (change);
  }
}

void Avatar::pickup_weapon (Weapon &weapon) throw() {
  for (int w = 0; w < common->weapon_container_count; ++w) {
    if (weapon_containers[w].name == weapon.get_subtype()) {
      weapon_containers[w].weapon = &weapon;
      weapon_containers[w].equipped = true;
      equipped_weapons.push_back (&weapon);

      // If this is the first weapon we've picked up, arm it!
      if (weapon_mode == UNARMED) {
        weapon_mode = ARMED;
        weapon_index = w;
      }

      break;
    }
  }
}

int Avatar::get_shot_origin_x() const throw() {
  int r;
  WeaponContainer &wc = weapon_containers[weapon_index];
  if (direction == LEFT) {
    r = destination_rectangle->w - wc.shot_offset_x -
        wc.weapon->get_projectile_width();
  } else if (direction == RIGHT) {
    r = wc.shot_offset_x;
  }
  return r;
}

int Avatar::get_shot_origin_y() const throw() {
  WeaponContainer &wc = weapon_containers[weapon_index];
  return wc.shot_offset_y;
}

void Avatar::update (World *world, unsigned int tick) throw() {
  Controllers::Avatar *active_controller = get_controller();
  active_controller->update (world, *this, tick);

  bool fatal_tile_collision = false;

  // Update the state of this sprite.
  // Gravity's collision detection must be done separately, as moving forward
  // and trying to fall at the same time, when there's ground right under the
  // player, would move the player back to the previous location to get
  // "unstuck", thereby undoing the horizontal movement.
  if (world) { // Only fall if we've given the sprite a world to be in.
    if (state != JUMPING && state != FALLING) {
      destination_rectangle->y -= 1; // we want to see if Xeno can fall by even 1 pixel
      if (!world->is_colliding_with_environment (*this)) {
        set_state (FALLING);
        vertical_movement_start_y = destination_rectangle->y;
        vertical_movement_start_tick = tick;
      }
      destination_rectangle->y += 1;
    }
  }
  if (!moving_horizontally) {
    if (active_controller->is_left_pressed() && !active_controller->is_right_pressed()) {
      set_direction (LEFT);
      moving_horizontally = true;
      horizontal_movement_start_x = destination_rectangle->x;
      horizontal_movement_start_tick = tick;
    } else if (!active_controller->is_left_pressed() && active_controller->is_right_pressed()) {
      set_direction (RIGHT);
      moving_horizontally = true;
      horizontal_movement_start_x = destination_rectangle->x;
      horizontal_movement_start_tick = tick;
    }
  } else if ((active_controller->is_left_pressed() && active_controller->is_right_pressed()) ||
            (!active_controller->is_left_pressed() && !active_controller->is_right_pressed())) {
    moving_horizontally = false;
  }
  if (active_controller->is_jump_pressed() && state != JUMPING && state != FALLING) {
    set_state (JUMPING);
    vertical_movement_start_y = destination_rectangle->y;
    vertical_movement_start_tick = tick;
  } else if (state == JUMPING &&
             tick >= vertical_movement_start_tick + common->jump_duration) {
    set_state (FALLING);
    vertical_movement_start_y = destination_rectangle->y;
    vertical_movement_start_tick = tick;
  } else if (
      ((active_controller->is_left_pressed() && !active_controller->is_right_pressed()) ||
       (!active_controller->is_left_pressed() && active_controller->is_right_pressed())) &&
      state != JUMPING && state != FALLING) {
    set_state (RUNNING);
  } else if (state == RUNNING) { // and left and right not pressed
    set_state (STOPPING);
  } else if (state == STOPPING && get_times_state_played() >= 1.0F) {
    set_state (STILL);
  }

  // Move the sprite if it's in a world.
  blocked = false;
  if (world) {
    if (moving_horizontally) {
      int old_x = destination_rectangle->x;
      int dist = (tick - horizontal_movement_start_tick) *
                 common->speed_per_second / 1000;
      if (direction == LEFT) {
        destination_rectangle->x = horizontal_movement_start_x - dist;
      } else if (direction == RIGHT) {
        destination_rectangle->x = horizontal_movement_start_x + dist;
      }
      // If there's a horizontal collision...
      if (world->is_colliding_with_environment (*this, &fatal_tile_collision)) {
        int old_y = destination_rectangle->y;
        destination_rectangle->y += 25 / 2; // TODO: Remove hard coded tile size.
        if (state == RUNNING && !world->is_colliding_with_environment (*this)) {
          // The avatar takes a small step up.
          do {
            --destination_rectangle->y;
          } while (!world->is_colliding_with_environment (*this));
          ++destination_rectangle->y;
        } else {
          // The avatar hit a wall.
          destination_rectangle->x = old_x;
          destination_rectangle->y = old_y;
          moving_horizontally = false;
          blocked = true;
        }
      }
    }
    if (state == JUMPING) {
      int old_y = destination_rectangle->y;

      destination_rectangle->y = vertical_movement_start_y
          + dist_from_falling_with_gravity (common->jump_duration)
          - dist_from_falling_with_gravity (common->jump_duration -
          (tick - vertical_movement_start_tick));

      if (world->is_colliding_with_environment (*this, &fatal_tile_collision)) {
        // Start falling if Xeno hit the ceiling.
        destination_rectangle->y = old_y;
        set_state (FALLING);
        vertical_movement_start_tick = tick;
        vertical_movement_start_y = destination_rectangle->y;
      }
    }
    if (state == FALLING) {
      destination_rectangle->y = vertical_movement_start_y -
          dist_from_falling_with_gravity (tick - vertical_movement_start_tick);

      // Stop falling if the avatar hit the floor.
      if (world->is_colliding_with_environment (*this, &fatal_tile_collision)) {
        set_state (STILL);
        world->quake (common->weight);
        do {
          ++destination_rectangle->y;
        } while (world->is_colliding_with_environment (*this));
      }
    }
  } // if in a world

  if (weapon_mode == ARMED) {
    // Cycle through weapons.
    if (active_controller->is_weapon_previous_pressed() &&
        !active_controller->is_weapon_next_pressed()) {
      // Switch to the previous weapon.
      do {
        --weapon_index;
        if (weapon_index < 0) {
          weapon_index = common->weapon_container_count - 1;
        }
      } while (!weapon_containers[weapon_index].weapon);
      switch_weapon_sound.play();
    } else if (!active_controller->is_weapon_previous_pressed() &&
               active_controller->is_weapon_next_pressed()) {
      // Switch to the next weapon.
      do {
        ++weapon_index;
        if (weapon_index == common->weapon_container_count) {
          weapon_index = 0;
        }
      } while (!weapon_containers[weapon_index].weapon);
      switch_weapon_sound.play();
    } // select weapon

    // Fire the current weapon.
    if (world) { // If there's a world to fire the weapon in...
      if (active_controller->is_weapon_fire_pressed()) {
        weapon_containers[weapon_index].weapon->fire (*world, *this, tick);
      }
    }

    Animation *wha =
        weapon_containers[weapon_index].holding_animations[direction];
    if (wha) {
      wha->update (tick);
    }
    get_armed_weapon()->update (world, tick);
  }

  // Flash the sprite for a while after being hurt.
  if (recently_hurt) {
    recently_hurt = false;
    hurt_tick = tick;
  }
  if (hurt_tick != NOT_HURT) {
    static const int HURT_DURATION = 225; // in ms
    if (tick <= hurt_tick + HURT_DURATION) {
      static const int FLASH_DURATION = 75; // in ms
      damage_flicker = (tick / FLASH_DURATION) % 2;
    } else {
      hurt_tick = NOT_HURT;
      damage_flicker = false;
    }
  }

  if (health <= 0 || fatal_tile_collision) {
    kill (*world);
  }

  avatar_update (world, tick);
}

void Avatar::draw (Lighting *lighting) throw() {
  if (damage_flicker) {
    // RGB_d = RGB_s(A_s) + RGB_d(1)
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);
  }

  avatar_draw();

  if (weapon_mode == ARMED) {
    Animation *wha =
        weapon_containers[weapon_index].holding_animations[direction];
    if (wha) {
      wha->draw();
    }
    get_armed_weapon()->draw_flash (lighting);
  }

  // RGB_d = RGB_s(A_s) + RGB_d(1 - A_s)
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Draw a heavenly glow around gods.
  if (god) {
    glPushMatrix();
    glColor3f (0.5F, 0.5F, 0.5F);
    glTranslatef (destination_rectangle->w / 2,
                  destination_rectangle->h / 2,
                  0.0F);
    int radius = destination_rectangle->h * 2;
    lighting->add_light (radius);
    glPopMatrix();
  }
}

void Avatar::kill (World &world) throw() {
  // Create an explosion.
  if (common->explosion) {
    Explosion *explosion = new Explosion (*common->explosion);
    explosion->set_position (destination_rectangle->x,
                             destination_rectangle->y);
    explosion->set_team (common->team);
    world.add (*explosion);
  }

  // Drop pickups.
  for (std::list<PowerUp*>::const_iterator p = common->pickups.begin();
       p != common->pickups.end();
       ++p) {
    PowerUp *pickup = dynamic_cast<PowerUp*> ((*p)->clone());

    // Set the pick-up's position.
    Rectangle pickup_rect = pickup->get_position();
    int x = destination_rectangle->x + ((float)std::rand() / RAND_MAX) *
        (destination_rectangle->w - pickup_rect.w);
    int y = destination_rectangle->y;
    pickup->set_position (x, y);

    world.add (*pickup);
  }

  world.queue_command (death_command);
  Sprite::kill (world);
}
