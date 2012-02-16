/*
 * Tileset.h
 *
 *  Created on: 2011-12-19
 *      Author: samuel
 */

#ifndef TILESET_H
#define TILESET_H

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Tile.h"

namespace Game {
  namespace TileLayer {
    // Loads and stores a tileset.
    class Tileset {
      friend class Tile;
      friend class TilesetPalette;

    private:
      std::string name;
      int tile_size; // width and height in pixels

      sf::Image sfml_image; // for checking pixel based collisions

      GLuint texture;
      int texture_width, texture_height; // in pixels

      int tiles_wide, tiles_high;

      std::vector<Tile*> tiles;

    public:
      Tileset (const std::string &name) throw();
      virtual ~Tileset() throw();

      std::string get_name() const throw() { return name; }
      int get_tile_size() const throw() { return tile_size; }
      Tile *get_tile (int index) const throw() { return tiles[index]; }

      // Returns the tiles wide and high as laid out in the tileset texture.
      // Some tile locations may be empty.
      int get_tiles_wide() const throw() { return tiles_wide; }
      int get_tiles_high() const throw() { return tiles_high; }
    };
  } // namespace Tiles
} // namespace Game

#endif // TILESET_H
