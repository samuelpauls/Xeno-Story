/*
 * Map.h
 *
 *  Created on: 2011-08-16
 *      Author: samuel
 */

#ifndef MAP_H
#define MAP_H

#include <list>
#include <vector>
#include <string>
#include "../Rectangle.h"
#include "../TileLayer/TileLayout.h"
#include "../SpriteLayer/Sprites.h"
#include "../SpriteLayer/Sprite.h"
#include "../SpriteLayer/Avatar.h"
#include "../AvatarStatus.h"
#include "../World.h"
#include "Editor.h"
#include "Layer.h"

using Game::SpriteLayer::Sprite;
using Game::SpriteLayer::Avatar;

namespace Game {
  namespace Map {
    // An area of tile, sprite, image, and other layers that exists while the
    // player is in it.
    class Map {
      friend class Editor; // uses map layers

    private:
      std::string name;
      int tiles_wide, tiles_high;

      int quake_magnitude;

      // Tilesets used by this map.  Stored in the order of map-specific tileset
      // index.  For example, two maps may refer to a tileset with index 5, but
      // tileset 5 may not be the same tileset for both maps.  If tileset 5 were
      // the same for every map, then people would require a central repository
      // for indexing their new tilesets.
      std::vector<TileLayer::Tileset*> tilesets;

      // Layers
      std::list<Layer*> layers;
      TileLayer::TileLayout *mainground_tiles;
      SpriteLayer::Sprites *sprites;

      Lighting *lighting;
      std::list<AvatarStatus*> avatar_statuses;

    public:
      Map (const std::string &profile, const std::string &name, World &world);
      virtual ~Map();

      // Returns the human readable name of this map.
      std::string get_name() const throw() { return name; }

      // Executes a command that deals with this map.
      // Returns true if the command was executed or false if not.
      bool process_command (const std::string &command) throw();

      void quake (int magnitude) throw() { quake_magnitude = magnitude; }

      // If a boolean is provided to fatal, it's set to true if the collision is
      // fatal (eg spikes).
      bool is_colliding_with_environment (const Sprite &sprite, bool *fatal)
                                         const throw();

      // Returns the nearest enemy avatar, or NULL if there are no enemies.
      // If distance is not NULL, it is set to the distance between the sprites.
      SpriteLayer::Avatar *get_nearest_enemy (const SpriteLayer::Avatar &avatar,
                                          int *distance) const throw() {
        return sprites->get_nearest_enemy (avatar, distance);
      }

      SpriteLayer::Sprite *get_touching_sprite (int x, int y) const throw() {
        return sprites->get_touching_sprite (x, y);
      }

      std::list<SpriteLayer::Sprite*> get_touching_sprites (
          const Rectangle &rectangle) const throw() {
        return sprites->get_touching_sprites (rectangle);
      }

      SpriteLayer::Sprite *get_sprite_by_id (const std::string &id) const throw() {
        return sprites->get_by_id (id);
      }

      void attach_sprite (Sprite &sprite) { sprites->attach (&sprite); }
      void detatch_sprite (Sprite &sprite) throw () {
        sprites->detatch (sprite);
      }

      TileLayer::TileLayout *get_mainground_tiles() const throw() {
        return mainground_tiles;
      }

      int get_layer_count() const throw() { return layers.size(); }

      // Save this map to disk.
      void save (const std::string &profile) const throw();

      void update (World &world, unsigned int tick) throw();

      void set_sprite_id_visibility (bool visible) throw() {
        sprites->set_id_visibility (visible);
      }

      void draw (const Rectangle &viewport) throw();
    };
  } // Map namespace
}

#endif // MAP_H
