/*
 * World.h
 *
 *  Created on: 2011-08-21
 *      Author: samuel
 */

#ifndef WORLD_H
#define WORLD_H

#include <map> // for std::multimap
#include <list>
#include "Audio.h"
#include "Rectangle.h"
#include "SpriteLayer/Sprite.h"
#include "Camera.h"
#include "Map/Editor.h"
#include "TileLayer/Tilesets.h"
#include "Terminal.h"

namespace Game {
  class Achievement;
  class Title;
  class MessageBox;
  namespace Map {
    class Map;
  }
  class AvatarStatus;
  namespace SpriteLayer {
    class Avatar;
  }

  // Contains several maps, music, etc.  Great for passing to a srite's update
  // method so that it can modify or get anything of interest.
  class World {
  public:
    enum GameStatus {
      RUNNING = 0,
      PAUSED,
      PLAYER_LOST, // Occurs after the player dies.
      PLAYER_WON, // Occurs after the player beats the game.
    };

  private:
    // The player starts as NULL, is set with the "set_player" command, and
    // returns to NULL upon death.
    SpriteLayer::Avatar *player;

    AvatarStatus *player_status;
    Camera *camera;
    Achievement *achievement;
    Title *title;

    static const std::string MUSIC_PATH;
    Audio *music;

    static const std::string DEFAULT_PROFILE;
    std::string profile;

    Map::Map *current_map;
    std::list<Map::Map*> maps;

    // All unique tilesets ever used by this world.
    TileLayer::Tilesets tilesets;

    MessageBox *message_box;
    bool message_box_toggling_off;

    Terminal *terminal;

    unsigned int world_tick; // in ms, doesn't increase while paused
    static const unsigned int UNSET = 0;
    unsigned int last_frame_tick; // in ms

    // The map editor is part of the world because its state (selected brush and
    // layer) persists between maps.
    Map::Editor *editor;
    bool editor_toggled;
    std::string player_map_before_editing;
    void start_editor() throw();

    // Queue commands for execution at the start of the next world update.  Two
    // examples of the usefulness of this waiting include:
    // - When loading a new map, we don't want to delete the old one immediately
    //   as it may have sprites and other things left to update.
    // - When adding a map's initial sprites, we want the map to be otherwise
    //   ready to go.
    std::multimap<unsigned int, std::string> queued_commands;

    // Execute the commands that were queued during the last update, etc.
    // tick is in ms
    void execute_commands (unsigned int tick) throw();

    GameStatus status;

  public:
    World (const std::string &profile);
    virtual ~World();

    // Switch to the specified map, loading it if necessary.  A profile may
    // optionally be specified.  If the map doesn't exist in the profile, the
    // default is still used.
    void set_map (const std::string &map_name,
                  const std::string &profile = DEFAULT_PROFILE) throw();

    // Sets the player to a sprite that's within the map.
    void set_player (SpriteLayer::Avatar *avatar) throw();

    SpriteLayer::Avatar *get_player() const throw();

    // Returns the nearest enemy avatar, or NULL if there are no enemies.
    // If distance is not NULL, it is set to the distance between the sprites.
    SpriteLayer::Avatar *get_nearest_enemy (const SpriteLayer::Avatar &avatar,
                                        int *distance) const throw();

    // Returns the sprite touching the given x and y location or NULL.
    SpriteLayer::Sprite *get_touching_sprite (int x, int y) const throw();

    // Returns sprites that are distance close or closer.
    std::list<SpriteLayer::Sprite*> get_touching_sprites (
        const Rectangle &rectangle) const throw();

    // Queue a command for execution at the start of the next world update.
    void queue_command (const std::string &command) throw();

    // If a boolean is provided to fatal, it's set to true if the collision is
    // fatal (eg spikes).
    bool is_colliding_with_environment (const SpriteLayer::Sprite &sprite,
                                        bool *fatal = NULL)
                                       const throw();

    void hit_tiles (const Rectangle &rectangle) throw();

    void add (Sprite &sprite) throw();

    Camera *get_camera() { return camera; }

    void quake (int magnitude) throw();

    void teleport_player (const std::string &map_name, int x, int y) throw();

    void teleport_player (const std::string &map_name,
                          const std::string &door_id) throw();

    // tick is in ms
    void update (unsigned int tick) throw();

    void detatch_sprite (Sprite &sprite);

    void draw() throw();

    // Returns the status of this world such as RUNNING or PLAYER_LOST.
    GameStatus get_status() const throw() { return status; }

    // Resume the world after it was paused.
    void resume() throw();

    TileLayer::Tilesets &get_tilesets() throw() { return tilesets; }

    // Save all open maps.  If the editor is open, the maps are
    // saved as the defaults to be used when players load areas for the first
    // time and after editing.  If the editor is closed, the maps are saved to
    // the player's profile.
    void save() throw();
  };
}

#endif // WORLD_H
