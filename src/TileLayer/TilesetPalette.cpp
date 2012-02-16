/*
 * TilesetPalette.cpp
 *
 *  Created on: 2011-12-20
 *      Author: samuel
 */

#include <vector>
#include "../Controllers/Manager.h"
#include "../Controllers/Editor.h"
#include "TilesetPalette.h"

using Game::TileLayer::TilesetPalette;

TilesetPalette::TilesetPalette() throw() {
  tileset = NULL;
  tile_layer = NULL;
  selected_tile = NULL;
}

TilesetPalette::~TilesetPalette() throw () {
  delete tile_layer;
}

void TilesetPalette::set_tileset (Tileset* tileset) throw() {
  if (this->tileset != tileset) {
    this->tileset = tileset;

    // Create a tile layer to display the tiles and allow for copying.
    std::vector<Tileset*> tilesets;
    tilesets.push_back (tileset);
    tile_layer = new TileLayout ("", tilesets, "", "",
                          tileset->get_tiles_wide(), tileset->get_tiles_high());

    // Fill the palette with tiles.
    for (int y = 0; y < tileset->get_tiles_high(); ++y) {
      for (int x = 0; x < tileset->get_tiles_wide(); ++x) {
        for (unsigned int t = 0; t < tileset->tiles.size(); ++t) {
          Tile *tile = tileset->get_tile (t);
          if (tile->get_x_index() == x && tile->get_y_index() == y) {
            tile_layer->set_tile (x, y, tile);
            break;
          }
        }
      }
    }

    selected_tile = tile_layer->get_tile (0, 0);
  } // if a new tileset was specified
}

void TilesetPalette::update() throw() {
  static Controllers::Editor *ec =
      Controllers::Manager::get_editor_controller();

  // Select a tile by hot key.
  if (ec->is_next_brush_pressed()) {
    int tile_x_index = selected_tile->get_x_index();
    int tile_y_index = selected_tile->get_y_index();
    if (++tile_x_index == tileset->get_tiles_wide()) {
      tile_x_index = 0;
      if (++tile_y_index == tileset->get_tiles_high()) {
        tile_y_index = 0;
      }
    }
    selected_tile = tile_layer->get_tile (tile_x_index, tile_y_index);
  }
  if (ec->is_previous_brush_pressed()) {
    int tile_x_index = selected_tile->get_x_index();
    int tile_y_index = selected_tile->get_y_index();
    if (tile_x_index-- == 0) {
      tile_x_index = tileset->get_tiles_wide() - 1;
      if (tile_y_index-- == 0) {
        tile_y_index = tileset->get_tiles_high() - 1;
      }
    }
    selected_tile = tile_layer->get_tile (tile_x_index, tile_y_index);
  }
}

void TilesetPalette::draw() const throw() {
  glPushMatrix();
  glTranslatef (location.x, location.y, 0.0F);

  tile_layer->draw();

  // Draw a box around the selected image in the palette.
  if (selected_tile) {
    glColor3ub (156, 120, 152); // light purple
    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    int tile_size = selected_tile->get_tileset()->get_tile_size();
    int tile_x_pixel = selected_tile->get_x_index() * tile_size;
    int tile_y_pixel = selected_tile->get_y_index() * tile_size;
    glRecti (tile_x_pixel, tile_y_pixel,
             tile_x_pixel + tile_size, tile_y_pixel + tile_size);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
  }

  glPopMatrix();
}

void TilesetPalette::mouse_primary_pressed (int x, int y) throw() {
  int tile_x = (x - location.x) / tile_layer->get_tile_size();
  int tile_y = (y - location.y) / tile_layer->get_tile_size();
  selected_tile = tile_layer->get_tile (tile_x, tile_y);
}
