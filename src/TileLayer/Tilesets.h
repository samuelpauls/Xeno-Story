/*
 * Tilesets.h
 *
 *  Created on: 2011-12-19
 *      Author: samuel
 */

#ifndef TILESETS_H
#define TILESETS_H

#include <string>
#include <list>
#include "Tileset.h"

namespace Game {
  namespace TileLayer {
    // Provides tilesets, caching along the way.
    class Tilesets {
    private:
      std::list<Tileset*> tilesets;

    public:
      Tilesets() throw();
      virtual ~Tilesets() throw();

      Tileset *get_tileset (const std::string &name) throw();
    };
  } // namespace Tiles
} // namespace Game

#endif // TILESETS_H
