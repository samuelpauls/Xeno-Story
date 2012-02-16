/*
 * Gate.h
 *
 *  Created on: 2011-10-14
 *      Author: samuel
 */

#ifndef GATE_H
#define GATE_H

#include <string>
#include "../Animation.h"
#include "../Audio.h"
#include "../Lighting.h"
#include "../World.h"
#include "Sprite.h"

namespace Game {
  namespace SpriteLayer {
    // Gates keep an avatar out of a given area until it has a certain weapon
    // that can shoot the particular type of gate down.
    class Gate: public Sprite {
    private:
      // Data used by all gates.
      static Audio unbreakable_sound;
      static Audio open_sound;

      // Data used by this gate and clones of the same subtype.
      struct Common {
        Animation *closed_animation;
        std::string weakness; // name of projectile that opens this gate
        
        int reference_count;
      } *common;

    public:
      Gate (const std::string &subtype) throw();
      Gate (const Gate &gate) throw();
      virtual ~Gate() throw();

      // Copy this gate.
      virtual Sprite *clone() const throw() { return new Gate (*this); }

      // Update the gate's state.  (animation, etc)
      // tick is ms since program start.
      virtual void update (World *world, unsigned int tick) throw();

      // This gate can be opened by a certain type of projectile.
      void affect (World &world, Sprite &sprite) throw();

      // Draw the gate.
      // If lighting is NULL, the gate is to be drawn without lighting
      // considerations.
      virtual void draw (Lighting *lighting) throw();
    }; // Gate
  } // namespace SpriteLayer
} // namespace Game

#endif // GATE_H
