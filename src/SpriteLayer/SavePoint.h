/*
 * SavePoint.h
 *
 *  Created on: 2011-10-21
 *      Author: samuel
 */

#ifndef SAVEPOINT_H
#define SAVEPOINT_H

#include <string>
#include "../Animation.h"
#include "../Lighting.h"
#include "../World.h"
#include "Sprite.h"

namespace Game {
  namespace SpriteLayer {
    // Saves the world so that the player doesn't have to redo his/her work
    // after death.
    class SavePoint: public Sprite {
    private:
      // Data shared between cloned subtypes.
      struct Common {
        Animation *idle, *activated;

        int reference_count;
      } *common;

      Animation *current_animation;

    public:
      // subtype could be something like "tent" or "kiosk"
      SavePoint (const std::string &subtype) throw();

      SavePoint (const SavePoint &save_point) throw();

      virtual ~SavePoint() throw();

      Sprite *clone() const throw() { return new SavePoint (*this); }


      // Update the sprite's state.  (animation, etc)
      // tick is ms since program start.
      virtual void update (World *world, unsigned int tick) throw();

      // Save the world when activated by an avatar.
      void affect (World &world, Sprite &sprite) throw();

      // Draw the sprite.
      // If lighting is NULL, the sprite is to be drawn without lighting
      // considerations.
      virtual void draw (Lighting *lighting) throw();
    }; // class SavePoint
  } // namespace SpriteLayer
} // namespace Game

#endif // SAVEPOINT_H
