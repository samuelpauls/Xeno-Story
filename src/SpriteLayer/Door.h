/*
 * Door.h
 *
 *  Created on: 2011-10-25
 *      Author: samuel
 */

#ifndef DOOR_H
#define DOOR_H

#include <string>
#include "../Animation.h"
#include "../World.h"
#include "Sprite.h"

namespace Game {
  namespace SpriteLayer {
    class Door: public Sprite {
    private:
      // The player can only use doors if s/he was previously touching one
      // without activating it.  This prevents going through a door, and since
      // the other touching door is immediately activated too, travelling back
      // to the original door.
      static bool can_use_doors;

      // Data shared among clones.
      struct Common {
        Animation *open_animation, *closed_animation;
        int reference_count;
      } *common;

      std::string destination_map_name, destination_door_id;
      bool open;

      void register_properties() throw();

    public:
      Door (const std::string &subtype) throw();
      Door (const Door &Door) throw();
      virtual ~Door() throw();

      Sprite *clone() const throw() { return new Door (*this); }

      void set_open (bool open) throw() { this->open = open; }

      // Update the sprite's state.  (animation, etc)
      // tick is ms since program start.
      virtual void update (World *world, unsigned int tick) throw();

      // This sprite affects the provided sprite argument.
      void affect (World &world, Sprite &sprite) throw();

      // Draw the sprite.
      // If lighting is NULL, the sprite is to be drawn without lighting
      // considerations.
      virtual void draw (Lighting *lighting) throw();
    }; // Door
  } // Sprites
} // Game

#endif // DOOR_H
