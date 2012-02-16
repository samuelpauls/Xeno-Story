/*
 * Torch.h
 *
 *  Created on: 2011-09-11
 *      Author: samuel
 */

#ifndef TORCH_H
#define TORCH_H

#include <string>
#include "../Animation.h"
#include "../World.h"
#include "../Lighting.h"
#include "Sprite.h"

namespace Game {
  namespace SpriteLayer {
    class Torch: public Sprite {
    private:
      Animation *out_animation, *lit_animation;
      bool lit;

      int previous_tick; // in ms
      int ticks_since_flicker; // ms since last flicker

      int light_radius;

      void register_properties() throw();

    public:
      Torch (const std::string &subtype) throw();
      Torch (const Torch &torch) throw();
      virtual ~Torch() throw();

      Sprite *clone() const throw() { return new Torch (*this); }

      // Update the sprite's state.  (animation, etc)
      // tick is ms since program start.
      virtual void update (World *world, unsigned int tick) throw();

      // This torch can be lit by projectiles.
      void affect (World &world, Sprite &sprite) throw();

      // Draw the sprite.
      // If lighting is NULL, the sprite is to be drawn without lighting
      // considerations.
      virtual void draw (Lighting *lighting) throw();
    }; // class Torch
  } // namespace SpriteLayer
} // namespace Game

#endif // TORCH_H
