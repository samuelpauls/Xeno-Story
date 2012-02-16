/*
 * SmallAvatar.cpp
 *
 *  Created on: 2011-08-13
 *      Author: samuel
 */

#include <fstream>
#include <cstdlib> // std::malloc
#include "SmallAvatar.h"

using Game::SpriteLayer::SmallAvatar;

int SmallAvatar::get_animation_index (int direction,
                                 int state, int weapon_mode)  const throw() {
  return weapon_mode * (DIRECTION_COUNT * STATE_COUNT) +
         direction * STATE_COUNT +
         state;
}

void SmallAvatar::avatar_draw() const throw() {
  current_animation->draw();
}

SmallAvatar::SmallAvatar (const std::string &subtype) throw():
    Avatar (subtype) {
  // Setup avatar animations.
  animations = reinterpret_cast<Animation**> (
      std::malloc (sizeof (Animation*) * ANIMATION_COUNT));
  for (int wm = 0; wm < WEAPON_MODE_COUNT; ++wm) {
    std::string weapon_mode_name;
    if (wm == UNARMED) {
      weapon_mode_name = "unarmed";
    } else {
      weapon_mode_name = "armed";
    }
    for (int d = 0; d < DIRECTION_COUNT; ++d) {
      for (int s = 0; s < STATE_COUNT; ++s) {
        std::string filename = "sprites/" + subtype + "/" +
            weapon_mode_name + "/" + STATE_NAMES[s] + ".png";
        std::ifstream f (filename.c_str());
        int a = get_animation_index (d, s, wm);
        if (f) { // If animation file exists...
          f.close();
          animations[a] = new Animation (filename);
        } else { // If the animation file doesn't exist...
          if (wm == ARMED) {
            // While this sprite doesn't have armed graphics, we can still use
            // the unarmed variants.
            animations[a] = animations[get_animation_index(d, s, UNARMED)];
          } else {
            animations[a] = NULL;
          }
        }
      } // state
    } // direction
    for (int s = 0; s < STATE_COUNT; ++s) {
      Animation *left_anim = animations[get_animation_index (LEFT, s, wm)];
      if (left_anim) {
        left_anim->flip_horizontally();

        // When running right and stopping, a dust cloud may form to the left of
        // the avatar.  This requires that the stopping right animation has an x
        // offset so the avatar doesn't lurch.  However, when running left,
        // the dust cloud forms to the right of the stopping avatar and no x
        // offset is needed to keep the avatar from lurching.
        left_anim->set_draw_offset_x (0);
      } else {
        for (int d = 0; d < DIRECTION_COUNT; ++d) {
          animations[get_animation_index (d, s, wm)] =
              animations[get_animation_index (d, STILL, wm)];
        }
      }
    }
  } // weapon mode
  current_animation = animations[get_animation_index (RIGHT, STILL, UNARMED)];

  set_size (current_animation->get_width(), current_animation->get_height());
}

SmallAvatar::SmallAvatar (const SmallAvatar &small_avatar) throw():
    Avatar(small_avatar) {
  animations = reinterpret_cast<Animation**> (
      std::malloc (sizeof (Animation*) * ANIMATION_COUNT));
  for (int a = 0; a < ANIMATION_COUNT; ++a) {
    animations[a] = new Animation (*small_avatar.animations[a]);
  }
  current_animation = animations[
      get_animation_index (get_direction(), get_state(), get_weapon_mode())];
}

SmallAvatar::~SmallAvatar() throw() {
  // Delete all animations.
  bool has_armed_animations =
      animations[get_animation_index (LEFT, STILL, UNARMED)] !=
      animations[get_animation_index (LEFT, STILL, ARMED)];
  for (int wm = 0; wm < WEAPON_MODE_COUNT; ++wm) {
    if (wm == UNARMED || has_armed_animations) {
      for (int d = 0; d < DIRECTION_COUNT; ++d) {
        for (int s = 0; s < STATE_COUNT; ++s) {
          int a = get_animation_index (d, s, wm);
          if (animations[a] != animations[get_animation_index (d, STILL, wm)]) {
            delete animations[a];
          }
        }

        // Animations that aren't provided for this avatar default to the still
        // animation.  As there may be several states that use the same still
        // animation, we don't delete still animations above.  Instead, we delete
        // the still animation only once for each direction at this time.
        delete animations[get_animation_index (d, STILL, wm)];
      }
    }
  }
  std::free (animations);
}

void SmallAvatar::avatar_update (World *world, unsigned int tick) throw() {
  // Switch to a different animation if necessary.
  Animation *new_anim = animations[get_animation_index (get_direction(),
                                                        get_state(),
                                                        get_weapon_mode())];
  if (current_animation != new_anim) {
    current_animation = new_anim;
    current_animation->rewind (tick);
  } else {
    current_animation->update (tick);
  }
}
