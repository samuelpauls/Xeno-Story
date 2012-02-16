/*
 * Explosion.h
 *
 *  Created on: 2011-10-10
 *      Author: samuel
 */

#include <string>
#include "../Audio.h"
#include "../Animation.h"
#include "../Lighting.h"
#include "../World.h"
#include "Sprite.h"

#ifndef EXPLOSION_H
#define EXPLOSION_H

namespace Game {
  namespace SpriteLayer {
    class Explosion: public Sprite {
    private:
      // Data shared among cloned subtypes.
      struct Common {
        Animation *animation;
        Audio *sound;

        int reference_count;
      } *common;

      int magnitude;

      int team;

      int previous_tick; // in ms
      int next_part_tick; // in ms
      int spawn_wait; // in ms
      std::list<Animation*> parts;

      void register_properties() throw();

    public:
      Explosion (const std::string &subtype) throw();
      Explosion (const Explosion &explosion) throw();
      virtual ~Explosion() throw();

      Sprite *clone() const throw() { return new Explosion (*this); }

      void set_magnitude (int magnitude) throw() {
        this->magnitude = magnitude;
      }

      void set_size (int width, int height) {
        destination_rectangle->w = width;
        destination_rectangle->h = height;
      }

      void set_team (int team) throw() { this->team = team; }

      // Update the sprite's state.  (animation, etc)
      // tick is ms since program start.
      virtual void update (World *world, unsigned int tick) throw();

      // This explosion affects the specified sprite.
      void affect (World &world, Sprite &sprite) throw();

      // Draw the sprite.
      // If lighting is NULL, the sprite is to be drawn without lighting
      // considerations.
      virtual void draw (Lighting *lighting) throw();
    };
  } // namespace SpriteLayer
} // namespace Game
#endif // EXPLOSION_H
