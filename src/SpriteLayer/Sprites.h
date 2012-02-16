/*
 * Sprites.h
 *
 *  Created on: 2011-08-15
 *      Author: samuel
 */

#ifndef SPRITES_H
#define SPRITES_H

#include <list>
#include <string>
#include "../Rectangle.h"
#include "../Font.h"
#include "../Map/Layer.h"
#include "../Lighting.h"
#include "Sprite.h"
#include "Avatar.h"

namespace Game {
  class World;

  namespace SpriteLayer {
    // Responsible for populating the environment with sprites and telling them
    // when to interact with each other.
    class Sprites: public Map::Layer {
    private:
      std::list<Sprite*> sprites;

      // Update iterators (including the affecting portion), may be incremented
      // by the detatch method before they would otherwise become invalid.  If
      // incremented during detatchment, the respective iterator isn't
      // incremented at the end of the respective update iteration.
      std::list<Sprite*>::iterator iterator, iterator_b;
      bool iterator_incremented, iterator_b_incremented;
      bool updating;

      bool id_visibility; // If true, IDs are drawn with sprites.
      static Font id_text;

    public:
      Sprites (const std::string &data_filename) throw();
      virtual ~Sprites() throw();

      // Executes a command dealing with sprites.
      // Returns true if the command was executed or false if not.
      bool process_command (const std::string &command) throw();

      // The sprite is automatically deleted after Sprites is done with it.
      void attach (Sprite *sprite) throw();

      void detatch (Sprite &sprite) throw();

      Sprite *get_by_id (const std::string &id) const throw();

      // Returns the nearest enemy avatar, or NULL if there are no enemies.
      // If distance is not NULL, it is set to the distance between the sprites.
      Avatar *get_nearest_enemy (const Avatar &avatar,
                                 int *distance) const throw();

      // Returns a sprite touching the specified sprite or NULL.
      std::list<Sprite*> get_touching_sprites (const Sprite &sprite)
                                              const throw();

      // Return the sprite touching the x and y location or NULL.
      Sprite *get_touching_sprite (int x, int y) const throw();

      std::list<Sprite*> get_touching_sprites (const Rectangle &rectangle)
                                              const throw();

      // Update the sprites states.  (animation, etc)
      // tick is ms since program start
      virtual void update (World &world, unsigned int tick) throw();

      // If set to true, IDs are drawn with their respective sprites.
      void set_id_visibility (bool visible) throw() { id_visibility = visible; }

      // Draw the sprite.
      void draw (Lighting &lighting, const Rectangle &viewport) throw();

      // Save this sprites layer to disk.
      virtual void save (const std::string &map_path) throw();
    };
  }
}

#endif // SPRITES_H
