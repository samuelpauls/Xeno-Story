/*
 * AI.cpp
 *
 *  Created on: 2011-08-21
 *      Author: samuel
 */

#include <cstdlib> // has std::rand
#include <cmath>
#include <climits> // has INT_MAX
#include "../SpriteLayer/Weapon.h"
#include "AI.h"

using Game::Controllers::AI;

void AI::initialize() throw() {
  left_pressed = false;
  right_pressed = false;
  jump_pressed = false;

  weapon_next = false;
  weapon_fire = false;

  change_tick = 0;
}

AI::AI() throw() {
  initialize();
}

AI::AI (const AI &ai) throw() {
  initialize();
}

AI::~AI() throw() {}

void AI::update (const World *world, const SpriteLayer::Avatar &controlled_avatar,
                 unsigned int tick) throw() {
  int dist_to_enemy;
  SpriteLayer::Avatar *enemy;

  // Gather information about the controlled avatar's environment.
  if (world) {
    enemy = world->get_nearest_enemy (controlled_avatar, &dist_to_enemy);
  } else {
    enemy = NULL;
    dist_to_enemy = INT_MAX;
  }

  // Act on the information gathered from the controlled avatar's environment.
  static const int SIGHT_DISTANCE = 300; // in pixels
  if (dist_to_enemy <= SIGHT_DISTANCE) {
    const SpriteLayer::Weapon *best_weapon = NULL;

    // Determine which weapon fires the least distance but still hits the
    // nearest enemy.
    int min_range = INT_MAX;
    for (int w = 0; w < controlled_avatar.get_equipped_weapon_count(); ++w) {
      const SpriteLayer::Weapon *weapon = controlled_avatar.get_equipped_weapon (w);
      int weapon_range = weapon->get_shot_distance();
      if (dist_to_enemy <= weapon_range) {
        if (weapon_range < min_range) {
          best_weapon = weapon;
          min_range = weapon_range;
        }
      }
    }

    if (best_weapon) {
      if (controlled_avatar.get_armed_weapon() != best_weapon) {
        // Select the best weapon.
        weapon_next = true;

        weapon_fire = false;
      } else {
        weapon_next = false;

        // Face the nearest enemy.
        if (controlled_avatar.get_direction() == SpriteLayer::Avatar::RIGHT) {
          if (enemy->get_position().x >
              controlled_avatar.get_position().get_right()) {
            left_pressed = false;
          } else {
            left_pressed = true;
          }
          right_pressed = false;
        } else { // If the controlled avatar is facing left...
          left_pressed = false;
          if (enemy->get_position().get_right() <
              controlled_avatar.get_position().x) {
            right_pressed = false;
          } else {
            right_pressed = true;
          }
        }

        // Fire the current weapon.
        weapon_fire = true;
      }
    } else { // If there's no best weapon...
      // Follow the player.
      if (controlled_avatar.get_position().x > enemy->get_position().x) {
        left_pressed = true;
        right_pressed = false;
      } else if (controlled_avatar.get_position().x < enemy->get_position().x) {
        left_pressed = false;
        right_pressed = true;
      }
      weapon_fire = false;
    }
  } else if (dist_to_enemy > SIGHT_DISTANCE) {
    // Wander left and right.
    if (tick >= change_tick) {
      // Choose a direction to wander.
      int num = std::rand() % 3;
      if (num == 0) { // 33% chance
        left_pressed = true;
        right_pressed = false;
      } else if (num == 1) { // 33% chance
        left_pressed = false;
        right_pressed = true;
      } else if (num == 2) { // 33% chance
        left_pressed = false;
        right_pressed = false;
      }
      weapon_fire = false;

      // This action will occur for 1 to 5 seconds.
      change_tick = tick + std::rand() % 4000 + 1000;
    }
  }

  // Jump if walking and not moving anywhere.
  jump_pressed = controlled_avatar.is_blocked();
}
