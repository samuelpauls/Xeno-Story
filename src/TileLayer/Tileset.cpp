/*
 * Tileset.cpp
 *
 *  Created on: 2011-12-19
 *      Author: samuel
 */

#include <fstream>
#include "../StringTokenizer.h"
#include "../Rectangle.h"
#include "Tileset.h"

using Game::TileLayer::Tileset;

Tileset::Tileset (const std::string &name) throw() {
  this->name = name;

  // Load the PNG into the image buffer.
  std::string texture_filename = "tiles/" + name + ".png";
  sfml_image.LoadFromFile (texture_filename);
  texture_width = sfml_image.GetWidth();
  texture_height = sfml_image.GetHeight();

  // Store the image buffer in an OpenGL texture.
  glGenTextures (1, &texture);
  glBindTexture (GL_TEXTURE_2D, texture);
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA,
                texture_width, texture_height,
                0, GL_RGBA, GL_UNSIGNED_BYTE,
                sfml_image.GetPixelsPtr());
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  std::string data_filename = "tiles/" + name + ".dat";
  std::ifstream f (data_filename.c_str());
  std::string str;

  // Get the tile width and height in pixels.
  std::getline (f, str);
  StringTokenizer tokens (str);
  tile_size = tokens.get_next_integer();

  tiles_wide = texture_width / tile_size;
  tiles_high = texture_height / tile_size;

  // Get the individual tile data (x index, y index).
  // Storing a tile's position in the tile's graphic file allows the tile to be
  // moved in the tile's graphic file without having to change that tile's index
  // in maps that use it.  If a tile's position in the tile graphic file wasn't
  // stored but rather sequentially calculated, moving it around in the tile
  // graphic file would cause its index to change and the map's that use it
  // would have to be modified to remain the same.  Shuffling tiles around in a
  // tile graphic file is useful because as tiles are added and removed, we may
  // need to arrange them differently so it's easier to see how nearby tiles
  // connect to them.
  while (!f.eof()) {
    int tile_index = tiles.size();

    std::getline (f, str);
    StringTokenizer tokens (str);
    Rectangle src_rect;
    src_rect.x = tokens.get_next_integer() * tile_size;
    src_rect.y = tokens.get_next_integer() * tile_size;
    src_rect.w = tile_size;
    src_rect.h = tile_size;

    bool dark = false;
    bool destructable = false;
    bool deadly = false;
    while (tokens.has_more_tokens()) {
      std::string attribute = tokens.get_next_string();
      if (attribute == "dark") {
        dark = true;
      } else if (attribute == "destructable") {
        destructable = true;
      } else if (attribute == "deadly") {
        deadly = true;
      }
    }

    Tile *tile = new Tile (tile_index, this, src_rect,
                           dark, destructable, deadly);
    tiles.push_back (tile);
  }

  f.close();
}

Tileset::~Tileset() throw() {
  // Delete the individual tiles.
  while (!tiles.empty()) {
    delete tiles.back();
    tiles.pop_back();
  }

  // Delete the tileset's shared data.
  glDeleteTextures (1, &texture);
}
