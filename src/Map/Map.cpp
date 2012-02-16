/*
 * Map.cpp
 *
 *  Created on: 2011-08-16
 *      Author: samuel
 */

#include <cstdlib> // for std::rand
#include <cassert>
#include <climits> // for INT_MAX
#include <fstream>
#include <typeinfo> // for typeid
#include <vector>
#include "../StringTokenizer.h"
#include "../FileSystem.h"
#include "../ImageLayer/ImageLayer.h"
#include "../Lighting.h"
#include "../SpriteLayer/Gate.h"
#include "../SpriteLayer/Elevator.h"
#include "Map.h"

using namespace Game::Map;

Map::Map::Map (const std::string &profile,
               const std::string &name,
               World &world) {
  this->name = name;

  std::string general_data = profile + '/' + name + "/general.dat";
  std::ifstream f (general_data.c_str());

  if (f) {
    // Get the map's width and height in tiles.
    std::string str;
    std::getline (f, str);
    StringTokenizer tokens (str);
    tiles_wide = tokens.get_next_integer();
    tiles_high = tokens.get_next_integer();

    // Get the map's tilesets.
    while (!f.eof()) {
      std::string tileset_name;

      // Read a tileset name.
      std::getline (f, tileset_name);
      unsigned int first_slash = tileset_name.find ('\r');
      if (first_slash != std::string::npos) {
        tileset_name = tileset_name.substr (0, first_slash);
      }

      tilesets.push_back (world.get_tilesets().get_tileset (tileset_name));
    }

    f.close();
  } else { // if file doesn't exist
    tiles_wide = 100;
    tiles_high = 100;
    tilesets.push_back (world.get_tilesets().get_tileset ("Grasslands"));
  }

  quake_magnitude = 0;

  TileLayer::TileLayout *tiles;
  TileLayer::TileLayout *backbackground_tiles;

  layers.push_back (new ImageLayer::ImageLayer (profile + '/' + name,
                                                "background_image.dat"));

  tiles = new TileLayer::TileLayout ("Back Background Tiles",
                                 tilesets,
                                 profile + '/' + name, "backbackground.dat",
                                 tiles_wide, tiles_high);
  layers.push_back (tiles);
  backbackground_tiles = tiles;

  tiles = new TileLayer::TileLayout ("Background Tiles",
                                 tilesets,
                                 profile + '/' + name, "background.dat",
                                 tiles_wide, tiles_high);
  layers.push_back (tiles);

  tiles = new TileLayer::TileLayout ("Mainground Tiles",
                                 tilesets,
                                 profile + '/' + name, "mainground.dat",
                                 tiles_wide, tiles_high);
  layers.push_back (tiles);
  mainground_tiles = tiles;

  sprites = new SpriteLayer::Sprites (profile + '/' + name);
  layers.push_back (sprites);

  tiles = new TileLayer::TileLayout ("Foreground Tiles",
                                 tilesets,
                                 profile + '/' + name, "foreground.dat",
                                 tiles_wide, tiles_high);
  layers.push_back (tiles);

  lighting = new Lighting (*backbackground_tiles);

  // If this map has startup commands, execute them.
  std::string start_commands_filename =
      profile + '/' + name + "/start_commands.dat";
  std::ifstream start_commands_file (start_commands_filename.c_str());
  if (start_commands_file) {
    start_commands_file.close();
    world.queue_command ("execute " + start_commands_filename);
  }
}

Map::Map::~Map() {
  // Delete avatar statuses.
  for (std::list<AvatarStatus*>::iterator as = avatar_statuses.begin();
       as != avatar_statuses.end();
       ++as) {
    delete *as;
  }

  // Delete every layer.
  for (std::list<Layer*>::iterator l = layers.begin();
       l != layers.end();
       ++l) {
    delete *l;
  }

  delete lighting;
}

bool Map::Map::process_command (const std::string &command) throw() {
  StringTokenizer tokens (command);
  std::string action = tokens.get_next_string();
  if (action == "monitor_avatar") {
    std::string sprite_id = tokens.get_next_string();
    Avatar *avatar = dynamic_cast<Avatar*> (sprites->get_by_id (sprite_id));
    assert (avatar);
    std::string avatar_status_type = tokens.get_next_string();
    AvatarStatus *avatar_status = new AvatarStatus (avatar_status_type);
    avatar_status->monitor (avatar);
    avatar_statuses.push_back (avatar_status);
    return true; // return executed
  } else if (action == "quake") {
    quake (tokens.get_next_integer());
    return true; // return executed
  } else {
    // Give the command to successive layers until one finally processes it.
    for (std::list<Layer*>::const_iterator l = layers.begin();
         l != layers.end();
         ++l) {
      bool executed = (*l)->process_command (command);
      if (executed) {
        return true; // return executed
      }
    }
  }
  return false; // return not executed
}

bool Map::Map::is_colliding_with_environment (const Sprite &sprite, bool *fatal)
                                             const throw() {
  bool touching_solid_sprite = false;

  // Is the sprite colliding with another solid sprite?
  std::list<Sprite*> touching_sprites = sprites->get_touching_sprites (sprite);
  for (std::list<Sprite*>::const_iterator s = touching_sprites.begin();
       s != touching_sprites.end();
       ++s) {
    if (typeid (**s) == typeid (SpriteLayer::Gate) ||
        typeid (**s) == typeid (SpriteLayer::Elevator)) {
      touching_solid_sprite = true;
      break;
    }
  } // for each touching sprite

  return get_mainground_tiles()->has_tiles (sprite.get_position(), fatal) ||
         touching_solid_sprite;
}

void Map::Map::save (const std::string &profile) const throw() {
  std::string map_path = profile + '/' + name;

  // The map path must be created before saving in it.
  FileSystem::create_path (map_path);

  // Save general map information.
  std::string filename = map_path + "/general.dat";
  std::ofstream f (filename.c_str());
  f << tiles_wide << ' ' << tiles_high;
  for (int ts = 0; ts < tilesets.size(); ++ts) {
    f << std::endl << tilesets[ts]->get_name();
  }
  f.close();

  // Save all the layers.
  for (std::list<Layer*>::const_iterator l = layers.begin();
       l != layers.end();
       ++l) {
    (*l)->save (map_path);
  }
}

void Map::Map::update (World &world, unsigned int tick) throw() {
  // Delete unused avatar statuses.
  for (std::list<AvatarStatus*>::iterator as = avatar_statuses.begin();
       as != avatar_statuses.end();
       ++as) {
    if ((*as)->get_avatar()->is_dead()) {
      delete *as;
      as = avatar_statuses.erase(as);
    }
  }

  // Update all the layers.
  for (std::list<Layer*>::iterator l = layers.begin();
       l != layers.end();
       ++l) {
    (*l)->update (world, tick);
  }

  // Update all the avatar statuses.
  for (std::list<AvatarStatus*>::iterator as = avatar_statuses.begin();
      as != avatar_statuses.end();
       ++as) {
    (*as)->update (tick);
  }
}

void Map::Map::draw (const Rectangle &viewport) throw() {
  glPushMatrix();

  // Carry out the next iteration of a quake.
  if (quake_magnitude) {
    glTranslatef (((float)std::rand() / INT_MAX - 0.5) * quake_magnitude,
                  ((float)std::rand() / INT_MAX - 0.5) * quake_magnitude,
                  0.0F);
    --quake_magnitude;
  }

  // Draw all the layers.
  // const_iterators can't change the referenced objects
  for (std::list<Layer*>::const_iterator l = layers.begin();
       l != layers.end();
       ++l) {
    (*l)->draw (*lighting, viewport);
  }

  lighting->draw (*lighting, viewport);

  // Draw all the avatar statuses.
  for (std::list<AvatarStatus*>::iterator as = avatar_statuses.begin();
      as != avatar_statuses.end();
       ++as) {
    (*as)->draw();
  }

  glPopMatrix();
}
