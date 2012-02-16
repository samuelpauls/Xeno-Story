/*
 * TilesEditor.h
 *
 *  Created on: 2011-08-17
 *      Author: samuel
 */

#ifndef TILESEDITOR_H
#define TILESEDITOR_H

#include "../Rectangle.h"
#include "../World.h"
#include "../Font.h"
#include "../UI/Manager.h"
#include "../UI/Frame.h"
#include "../UI/Menu.h"
#include "../Map/Layer.h"
#include "../Map/LayerEditor.h"
#include "TilesetPalette.h"
#include "TileLayout.h"

namespace Game {
  namespace TileLayer {
    // Allows the user to edit the layout of each layer of tiles.
    class TilesEditor: public Map::LayerEditor {
    private:
      TileLayout *tiles;
      int grid_x, grid_y; // x and y pixels of the first grid lines
      Rectangle *tile_hover_rectangle;

      TilesetPalette *tile_palette;
      UI::Frame *palette_frame;

      static Font tileset_name_text;
      UI::Menu *tilesets_menu;
      UI::Frame *tilesets_frame;

      // Must be static because this function is used as a callback by the menu.
      static void set_tileset (void *tiles_editor) throw();

    public:
      TilesEditor (UI::Manager &ui_manager) throw();

      virtual ~TilesEditor() throw();

      virtual void set_layer (Map::Layer &layer) throw();

      // Allows mouse to select different tiles and so on.
      // tick is in ms.
      void update (World &world, const Rectangle &viewport,
                   unsigned int tick) throw();

      void draw() throw();
    };
  } // namespace Tiles
}

#endif // TILESEDITOR_H
