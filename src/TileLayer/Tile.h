/*
 * Tile.h
 *
 *  Created on: 2011-12-19
 *      Author: samuel
 */

#ifndef TILE_H
#define TILE_H

#include "../Rectangle.h"

namespace Game {
  namespace TileLayer {
    class Tileset;

    class Tile {
    private:
      Tileset *tileset;
      int index;
      int x_index, y_index; // tile index from the bottom-left of the texture

      Rectangle source_rectangle; // pixels from top-left of tiles texture

      bool dark; // true for underground tiles that make the lighting dark
      bool destructable; // if true, can be shot away with weapons
      bool deadly; // if true, avatars that touch it die instantly

    public:
      Tile (int tile_index, Tileset *tileset, const Rectangle &source_rectangle,
            bool dark, bool destructable, bool deadly) throw();
      virtual ~Tile() throw();

      // Returns this tile's tileset.
      Tileset *get_tileset() const throw() { return tileset; }

      // Returns the index of this tile within its tileset.
      int get_index() const throw() { return index; }

      int get_x_index() const throw() { return x_index; }
      int get_y_index() const throw() { return y_index; }

      // Draw the tile with the bottom-left being at OpenGL's origin.
      void draw() const throw();

      bool is_dark() const throw() { return dark; }

      bool is_destructable() const throw() { return destructable; }

      // Deadly tiles should kill any touching avatar instantly.
      bool is_deadly() const throw() { return deadly; }

      // Returns true if a given x and y pixel into this tile is solid.
      bool is_pixel_solid (int x, int y) const throw();
    };
  } // namespace Tiles
} // namespace Game
#endif // TILE_H
