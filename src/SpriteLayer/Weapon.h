/*
 * Weapon.h
 *
 *  Created on: 2011-08-26
 *      Author: samuel
 */

#ifndef WEAPON_H
#define WEAPON_H

#include <string>
#include <vector>
#include "../Animation.h"
#include "../Audio.h"
#include "../World.h"
#include "Sprite.h"
#include "Avatar.h"

namespace Game {
  namespace SpriteLayer {
    class Weapon: public Sprite {
    private:
      // Data shared among copied weapons.
      struct Common {
        Animation *animation;
        Animation *flash; // muzzle flash when firing

        unsigned int fire_delay; // in ms
        Audio *fire_sound;

        struct Level {
          Sprite *shot_sprite;
          struct Shot {
            int delay; // in ms
            int offset_x, offset_y; // in pixels
            int angle; // in degrees
          };
          std::vector<Shot> shots;
          int range; // maximum distance in pixels
          std::string fire_command;
          int energy_to_next_level;
        };
        std::vector<Level> levels;

        int reference_count;
      } *common;

      Rectangle *flash_rect; // destination rects don't use w and h

      int shots_fired; // since beginning of the latest firing of this weapon
      const Avatar *firing_avatar; // avatar that recently fired this weapon
      unsigned int fire_tick; // in ms

      int level;
      int energy;

      void register_properties() throw();

    public:
      Weapon (const std::string &subtype) throw();
      Weapon (const Weapon &weapon) throw();
      virtual ~Weapon() throw();

      Sprite *clone() const throw() { return new Weapon (*this); }

      // Returns the width of the projectile this weapon fires in a shot at its
      // current level.
      int get_projectile_width() const throw() {
        return common->levels[level - 1].shot_sprite->get_position().w;
      }

      void affect_energy (int change) throw();
      int get_energy() const throw() { return energy; }

      // Returns the amount of energy required to reach the next weapon level.
      int get_full_energy() const throw() {
        return common->levels[level - 1].energy_to_next_level;
      }

      int get_level() const throw() { return level; }

      // Returns the maximum pixel distance of a shot from the fire origin at
      // this level.
      int get_shot_distance() const throw() {
        return common->levels[level - 1].range;
      }

      void fire (World &world, const Avatar &avatar, unsigned int tick) throw();

      // Update the sprite's state.  (animation, etc)
      // tick is ms since program start.
      virtual void update (World *world, unsigned int tick) throw();

      // This sprite affects the provided sprite argument.
      void affect (World &world, Sprite &sprite) throw();

      // Draw the sprite.
      // If lighting is NULL, the sprite is to be drawn without lighting
      // considerations.
      virtual void draw (Lighting *lighting) throw();

      // Draw the muzzle flash.
      // If lighting is NULL, the sprite is to be drawn without lighting
      // considerations.
      void draw_flash (Lighting *lighting) throw();
    }; // class Weapon
  } // namespace SpriteLayer
} // namespace Game

#endif // WEAPON_H
