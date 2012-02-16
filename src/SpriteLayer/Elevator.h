/*
 * Elevator.h
 *
 *  Created on: 2011-12-12
 *      Author: samuel
 */

#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <string>
#include "../Animation.h"
#include "../World.h"
#include "../Lighting.h"
#include "Path.h"
#include "PathFollower.h"
#include "Sprite.h"

namespace Game {
  namespace SpriteLayer {
    class Elevator: public Sprite {
    private:
      // Shared data
      struct Common {
        Animation *animation;
        int speed_per_second; // in pixels

        int reference_count;
      } *common;

      Path path;
      PathFollower path_follower;

      int x_end, y_end;

      void register_properties() throw();

      virtual void on_script_variable_changed (const std::string &name) throw();

    public:
      Elevator (const std::string &subtype) throw();
      Elevator (const Elevator &elevator) throw();
      virtual ~Elevator() throw();

      Sprite *clone() const throw() { return new Elevator (*this); }

      // Update the sprite's state.  (animation, etc)
      // tick is ms since program start.
      virtual void update (World *world, unsigned int tick) throw();

      // Draw the sprite.
      // If lighting is NULL, the sprite is to be drawn without lighting
      // considerations.
      virtual void draw (Lighting *lighting) throw();
    }; // class Elevator
  } // namespace SpriteLayer
} // namespace Game

#endif // ELEVATOR_H
