/*
 * Tiles.cpp
 *
 *  Created on: 2011-08-10
 *      Author: samuel
 */

#include <cstdlib> // has std::atoi
#include <cassert>
#include <cmath> // has std::ceil
#include <fstream>
#include <algorithm> // has std::min
#include "../StringTokenizer.h"
#include "TileLayout.h"

using Game::TileLayer::TileLayout;

void TileLayout::read_layout (const std::string &filename) throw() {
  // Iterate through the tiles and store them in the specified layer.
  std::ifstream f (const_cast<const char*>(filename.c_str()));
  if (f) {
    for (int tile_index_y = tiles_high - 1; tile_index_y >= 0; --tile_index_y) {
      // Tokenized numbers provide a simple way of allowing infinite tilesets
      // and tiles to be used in a map.
      std::string row_data;
      std::getline (f , row_data);
      StringTokenizer row_tokens (row_data);
      for (int tile_index_x = 0; tile_index_x < tiles_wide; ++tile_index_x) {
        std::string token = row_tokens.get_next_string();
        if (token == "*") {
          layout->set (tile_index_x, tile_index_y, NULL);
        } else {
          int tileset_index = std::atoi (token.c_str());
          int tile_index = row_tokens.get_next_integer();
          Tileset *tileset = tilesets[tileset_index];
          layout->set (tile_index_x, tile_index_y,
                       tileset->get_tile (tile_index));
        }
      } // for tile x index
    } // for tile y index
    f.close();
  } // layout file exists
}

TileLayout::TileLayout (const std::string &layer_name,
              const std::vector<Tileset*> &tilesets,
              const std::string &map_path,
              const std::string &layout_filename,
              int tiles_wide, int tiles_high) throw():
              Map::Layer (layer_name) {
  this->tilesets = tilesets;
  this->tiles_wide = tiles_wide;
  this->tiles_high = tiles_high;

  // Allocate space for the layout and start with no tiles in it.
  layout = new Layout2D<Tile*> (tiles_wide, tiles_high, NULL);

  if (layout_filename != "") {
    read_layout (map_path + '/' + layout_filename);
  }
  this->layout_filename = layout_filename;
}

TileLayout::~TileLayout() throw() {
  delete layout;
}

Game::Rectangle TileLayout::pixel_to_tile_rect (
    const Rectangle &pixel_rectangle) const throw() {
  Rectangle tile_rect;

  // Map the pixel rectangle to a tile rectangle.
  int tile_size = get_tile_size();
  tile_rect.x = pixel_rectangle.x / tile_size;
  tile_rect.y = pixel_rectangle.y / tile_size;
  int r = std::ceil ((float)pixel_rectangle.get_right() / tile_size);
  int t = std::ceil ((float)pixel_rectangle.get_top() / tile_size);
  tile_rect.w = r - tile_rect.x;
  tile_rect.h = t - tile_rect.y;

  // Clip the tile rectangle to the map.
  tile_rect.x = std::max (tile_rect.x, 0);
  tile_rect.y = std::max (tile_rect.y, 0);
  tile_rect.w = std::min (tile_rect.w, tiles_wide - tile_rect.x);
  tile_rect.h = std::min (tile_rect.h, tiles_high - tile_rect.y);

  return tile_rect;
}

void TileLayout::save (const std::string &map_path) throw() {
  // Open the layout file for writing.
  std::string filename = map_path + '/' + layout_filename;
  std::ofstream f;
  f.open (filename.c_str());

  // For each tile in the layer...
  for (int tile_index_y = tiles_high - 1; tile_index_y >= 0; --tile_index_y) {
    for (int tile_index_x = 0; tile_index_x < tiles_wide; ++tile_index_x) {
      // Write the tile to the file.
      if (tile_index_x > 0) {
        f << ' ';
      }
      Tile *tile = get_tile (tile_index_x, tile_index_y);
      if (tile == NULL) {
        f << '*';
      } else {
        unsigned int tileset_index;

        // Get the tile's tileset index relative to this map.
        for (tileset_index = 0;
             tileset_index < tilesets.size();
             ++tileset_index) {
          if (tilesets[tileset_index] == tile->get_tileset()) {
            break;
          }
        }

        f << tileset_index << ' ' << tile->get_index();
      }
    } // for tile x index
    f << std::endl;
  } // for tile y index

  f.close();
}

Game::TileLayer::Tile *TileLayout::get_tile (int x, int y) const throw() {
  return layout->get (x, y);
}

void TileLayout::set_tile (int x, int y, Tile *tile) throw() {
  layout->set (x, y, tile);
}

bool TileLayout::has_tiles (const Rectangle &pixel_rectangle, bool *fatal)
                           const throw() {
  for (int y = pixel_rectangle.y; y < pixel_rectangle.get_top(); ++y) {
    GLint tile_y = y / get_tile_size();
    if (tile_y < 0 || tile_y >= tiles_high) {
      break;
    }

    GLint tile_offset_y = (get_tile_size() - 1) - (y - tile_y * get_tile_size()); // sfml coords

    for (int x = pixel_rectangle.x; x < pixel_rectangle.get_right(); ++x) {
      GLint tile_x = x / get_tile_size();
      if (tile_x < 0 || tile_x >= tiles_wide) {
        break;
      }

      GLint tile_offset_x = x - tile_x * get_tile_size(); // sfml coords

      Tile *tile = get_tile (tile_x, tile_y);
      if (tile && tile->is_pixel_solid (tile_offset_x, tile_offset_y)) {
        if (fatal) {
          *fatal = tile->is_deadly();
        }
        return true; // collision
      }
    }
  }

  return false; // no collision
}

void TileLayout::hit (const Rectangle &rectangle) throw() {
  Rectangle tile_rect = pixel_to_tile_rect (rectangle);
  for (int y = tile_rect.y; y < tile_rect.get_top(); ++y) {
    for (int x = tile_rect.x; x < tile_rect.get_right(); ++x) {
      Tile *tile = get_tile (x, y);
      if (tile != NULL) {
        if (tile->is_destructable()) {
          set_tile (x, y, NULL);
        }
      }
    }
  }
}

void TileLayout::draw (Lighting &lighting, const Rectangle &viewport) throw() {
  if (viewport.x < -viewport.w || viewport.y < -viewport.h) {
    // The viewport is to the left of or above all tiles.
    return;
  }

  // Iterate through the tiles to be drawn.
  int tile_size = get_tile_size();
  Rectangle visible_tiles = pixel_to_tile_rect (viewport);
  for (int y = visible_tiles.y; y < visible_tiles.get_top(); ++y) {
    for (int x = visible_tiles.x; x < visible_tiles.get_right(); ++x) {
      Tile *tile = layout->get (x, y);
      if (tile) {
        glPushMatrix();
        glTranslatef (x * tile_size, y * tile_size, 0.0F);
        tile->draw();
        glPopMatrix();
      }
    } // x tile index
  } // y tile index
}

void TileLayout::draw() throw() {
  // Iterate through the tiles to be drawn.
  int tile_size = get_tile_size();
  for (int y = 0; y < tiles_high; ++y) {
    for (int x = 0; x < tiles_wide; ++x) {
      Tile *tile = layout->get (x, y);
      if (tile) {
        glPushMatrix();
        glTranslatef (x * tile_size, y * tile_size, 0.0F);
        tile->draw();
        glPopMatrix();
      }
    } // x tile index
  } // y tile index
}
