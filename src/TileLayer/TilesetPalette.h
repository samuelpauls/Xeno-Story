/*
 * TilesetPalette.h
 *
 *  Created on: 2011-12-20
 *      Author: samuel
 */

#ifndef TILESETPALETTE_H
#define TILESETPALETTE_H

#include "../UI/Widget.h"
#include "TileLayout.h"
#include "Tileset.h"

namespace Game {
  namespace TileLayer {
    class TilesetPalette: public UI::Widget {
    private:
      TileLayout *tile_layer;
      const Tileset *tileset;
      Tile *selected_tile;

    public:
      TilesetPalette() throw();
      virtual ~TilesetPalette() throw();

      void set_tileset (Tileset* tileset) throw();

      void update() throw();

      virtual void draw() const throw();

      // Called when the mouse primary clicks over this widget's location.
      // x and y are pixels from the widget's location's origin.
      virtual void mouse_primary_pressed (int x, int y) throw();

      void select_tile (Tile *tile) throw() { selected_tile = tile; }

      Tile *get_selected_tile() const throw() { return selected_tile; }
    }; // class TilesetPalette
  } // namespace Tiles
} // namespace Game

#endif // TILESETPALETTE_H
