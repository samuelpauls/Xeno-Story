/*
 * Projectile.h
 *
 *  Created on: 2011-08-20
 *      Author: samuel
 */

#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <string>
#include "Sprite.h"
#include "../World.h"
#include "../Lighting.h"

namespace Game {
  namespace SpriteLayer {
    class Projectile: public Sprite {
    private:
      // Data common to cloned projectiles of this subtype.
      struct Common {
        int speed; // in pixels per second
        int total_dist; // in pixels

        struct {
          int radius;
          float red, green, blue; // [0, 1]
        } light;

        bool rotate_animations; // if false, rotations only affect movement

        int reference_count;
      } *common;

      static const int NOT_SET = -1;
      int start_tick;

      int start_x, start_y; // in pixels

      double angle_degrees, angle_radians; // 0 is 3 o'clock, goes clockwise

      int team;

      Animation *current_animation;
      Animation *flying_animation;
      Animation *hit_animation;

      void register_properties() throw();

      virtual void on_script_variable_changed (const std::string &name) throw();

    public:
      Projectile (const std::string &subtype) throw();
      Projectile (const Projectile &projectile) throw();
      virtual ~Projectile() throw();

      Sprite *clone() const throw() { return new Projectile (*this); }

      // Returns the maximum pixel distance this projectile can travel.
      int get_maximum_distance() const throw() { return common->total_dist; }

      virtual void set_position (int x, int y) throw();

      // Set the angle in degrees, with 0 being 3 o'clock.
      void set_angle (double angle) throw();

      void set_team (int team) throw();

      // Update the sprite's state.  (animation, etc)
      // tick is ms since program start.
      virtual void update (World *world, unsigned int tick) throw();

      // This sprite affects the provided sprite argument.
      void affect (World &world, Sprite &sprite) throw();

      // Draw the sprite.
      // If lighting is NULL, the sprite is to be drawn without lighting
      // considerations.
      virtual void draw (Lighting *lighting) throw();
    };
  }
}

#endif // PROJECTILE_H
