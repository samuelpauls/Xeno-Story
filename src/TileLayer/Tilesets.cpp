/*
 * Tilesets.cpp
 *
 *  Created on: 2011-12-19
 *      Author: samuel
 */

#include "Tilesets.h"

using Game::TileLayer::Tilesets;

Tilesets::Tilesets() throw() {}

Tilesets::~Tilesets() throw() {
  // Delete each tileset.
  while (!tilesets.empty()) {
    delete tilesets.back();
    tilesets.pop_back();
  }
}

Game::TileLayer::Tileset *Tilesets::get_tileset (const std::string &name) throw() {
  // Try to return a cached copy of the requested tileset.
  for (std::list<Tileset*>::const_iterator ts = tilesets.begin();
       ts != tilesets.end();
       ++ts) {
    if ((*ts)->get_name() == name) {
      return *ts;
    }
  }

  // Load, cache, and return the requested tileset.
  Tileset *tileset = new Tileset (name);
  tilesets.push_back (tileset);
  return tileset;
}
