/*
 * PowerUp.h
 *
 *  Created on: 2011-10-14
 *      Author: samuel
 */

#ifndef POWER_UP_H
#define POWER_UP_H

#include <string>
#include "../Animation.h"
#include "../Audio.h"
#include "../Lighting.h"
#include "../World.h"
#include "Sprite.h"

namespace Game {
  namespace SpriteLayer {
    // Power-ups do things like increase an avatars maximum health.
    class PowerUp: public Sprite {
    private:
      // Data shared by all power-ups.
      static Audio pickup_sound;

      // Common data shared by cloned power-ups.
      struct Common {
        Animation *animation;
        
        int health_increase;
        int maximum_health_increase;
        int energy_increase;

        int reference_count;
      } *common;

    public:
      PowerUp (const std::string &subtype) throw();
      PowerUp (const PowerUp &power_up) throw();
      virtual ~PowerUp() throw();

      // Copy this power-up.
      virtual Sprite *clone() const throw() { return new PowerUp (*this); }

      // Update the power-up's state.  (animation, etc)
      // tick is ms since program start.
      virtual void update (World *world, unsigned int tick) throw();

      // This power-up can be picked up by an avatar.
      void affect (World &world, Sprite &sprite) throw();

      // Draw the power-up.
      // If lighting is NULL, the power-up is to be drawn without lighting
      // considerations.
      virtual void draw (Lighting *lighting) throw();
    };
  } // namespace SpriteLayer
} // namespace Game

#endif // POWER_UP_H
