/*
 * Layer.h
 *
 *  Created on: 2011-09-01
 *      Author: samuel
 */

#include <string>
#include "../Lighting.h"
#include "../Rectangle.h"
#include "../World.h"

#ifndef MAP_LAYER_H
#define MAP_LAYER_H

namespace Game {
  class Lighting;

  namespace Map {
    // A layer of the map that's updated and drawn during normal gameplay.  It
    // could be a sprite, tile, or other layer.
    class Layer {
    private:
      std::string name;

    public:
      // name is the human readable name of this map layer.
      Layer (const std::string &name) throw();

      virtual ~Layer() throw();

      // Processes a command dealing with this layer.
      // Returns true if the command was executed or false if not.
      virtual bool process_command (const std::string &command) throw() {
        return false; // command not executed
      };

      // Returns the human readable name of this map layer.
      std::string get_name() const throw() { return name; }

      virtual void update (World &world, unsigned int tick) throw() {}
      virtual void draw (Lighting &lighting, const Rectangle &viewport) throw() = 0;

      // Save this layer to disk.  Saving is the responsibility of the layers,
      // as opposed to the layer editors, as the level may change during regular
      // gameplay and we want to allow the player to save his or her progress.
      virtual void save (const std::string &map_path) throw() {}
    };
  } // Map namespace
}

#endif // MAP_LAYER_H
