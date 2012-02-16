/*
 * Tiles.h
 *
 *  Created on: 2011-08-10
 *      Author: samuel
 */

#ifndef TILELAYOUT_H
#define TILELAYOUT_H

#include <string>
#include <vector>
#include <GL/gl.h>
#include "../Rectangle.h"
#include "../Layout2D.h"
#include "../Window.h"
#include "../Map/Layer.h"
#include "Tileset.h"
#include "Tile.h"

namespace Game {
  class Lighting;

  namespace TileLayer {
    // Responsible for loading and displaying a 2D map of tiles.
    class TileLayout: public Map::Layer {
    private:
      // General tile information.
      int tiles_high, tiles_wide;
      std::string layout_filename;

      std::vector<Tileset*> tilesets;

      // Map tiles are laid out as an array of pointers to tiles.
      // Row zero is the bottom row of tiles in the map.  Row one is the next row
      // of tiles above and so on.
      Layout2D<Tile*> *layout;

      // Returns an index into the tile layout array that retrieves a tile pointer
      // for a specific layer and position.
      int get_layout_tile_index (int x, int y) const throw();

      void read_layout (const std::string &filename) throw();

    public:
      TileLayout (const std::string &layer_name,
             const std::vector<Tileset*> &tilesets,
             const std::string &map_path,
             const std::string &layout_filename,
             int tiles_wide, int tiles_high) throw();
      virtual ~TileLayout() throw();

      // Returns a rectangle of tile indices that correspond to the provided pixel
      // rectangle.  Pixel rectangles outside of the tile layout doesn't return tile
      // indices outside of the tile layout.  The returned indices are inclusive.
      Rectangle pixel_to_tile_rect (const Rectangle &pixel_rectangle) const throw();

      // Save the layout to disk.
      void save (const std::string &map_path) throw();

      // Returns the tile from the specified x and y index from the bottom-left.
      Tile *get_tile (int x, int y) const throw();

      // Sets a tile at the specified x and y index from the bottom-left.
      void set_tile (int x, int y, Tile *tile) throw();

      // Returns the number of tilesets used by this tile layer.
      int get_tileset_count() const throw() { return tilesets.size(); }

      Tileset *get_tileset (int index) { return tilesets[index]; }

      // Returns the number of tiles in the palette.
      int get_tile_count (int tileset) const throw();

      // True if there are any map tile pixels within this rectangle.
      // If a boolean is provided to fatal, it's set to true if any of the tiles
      // are deadly.
      bool has_tiles (const Rectangle &pixel_rectangle, bool *fatal)
                     const throw();

      // Destroy damageable tiles within the given rectangle of pixels.
      void hit (const Rectangle &rectangle) throw();

      // TODO: Use pointers instead with defaults of NULL.
      void draw (Lighting &lighting, const Rectangle &source_rectangle) throw();
      void draw() throw();

      // Returns one value representing the width and height of a tile in pixels.
      // Since all tiles in all the tilesets used by a given map are the same
      // size, we get the value arbitrarily from the first tileset in the map.
      // TODO: Move to map.
      int get_tile_size() const throw() {
        Tileset *tileset = tilesets[0];
        return tileset->get_tile_size();
      }

      // TODO: Move to map.
      int get_tiles_wide() const throw() { return tiles_wide; }

      // TODO: Move to map.
      int get_tiles_high() const throw() { return tiles_high; }
    };
  } // namespace Tiles
}

#endif // TILELAYOUT_H
