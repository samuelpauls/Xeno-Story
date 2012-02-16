/*
 * Rock.h
 *
 *  Created on: 2011-08-15
 *      Author: samuel
 */

#ifndef ROCK_H
#define ROCK_H

#include <string>
#include <list>
#include "../World.h"
#include "Sprite.h"

namespace Game {
  namespace SpriteLayer {
    class Rock: public Sprite {
    private:
      Animation *animation;

      std::string unique_interaction_command; // from map
      std::list<std::string> common_interaction_commands; // from subtype's data

      void register_properties() throw();

    public:
      Rock (const std::string &subtype) throw();
      Rock (const Rock &rock) throw();
      virtual ~Rock() throw();

      Sprite *clone() const throw() { return new Rock (*this); }

      // Update the sprite's state.  (animation, etc)
      // tick is ms since program start.
      virtual void update (World *world, unsigned int tick) throw();

      // This sprite affects the provided sprite argument.
      void affect (World &world, Sprite &sprite) throw();

      // Draw the sprite.
      // If lighting is NULL, the sprite is to be drawn without lighting
      // considerations.
      virtual void draw (Lighting *lighting) throw();
    }; // class Rock
  } // namespace SpriteLayer
} // namespace Game

#endif // ROCK_H
