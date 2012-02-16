/*
 * World.cpp
 *
 *  Created on: 2011-08-21
 *      Author: samuel
 */

#include <cstring> // has NULL
#include <cstdlib>
#include <fstream>
#include <string> // has std::getline
#include "Controllers/Manager.h"
#include "Controllers/Editor.h"
#include "Controllers/Player.h"
#include "Map/Map.h"
#include "AvatarStatus.h"
#include "Achievement.h"
#include "Title.h"
#include "StringTokenizer.h"
#include "SpriteLayer/Sprite.h"
#include "SpriteLayer/Avatar.h"
#include "SpriteLayer/Rock.h"
#include "SpriteLayer/Projectile.h"
#include "SpriteLayer/Door.h"
#include "MessageBox.h"
#include "FileSystem.h"
#include "World.h"

using Game::World;
using namespace Game::SpriteLayer;

const std::string World::DEFAULT_PROFILE = "maps";
const std::string World::MUSIC_PATH = "music/";

void World::save() throw() {
  std::string use_profile;

  if (editor) {
    use_profile = DEFAULT_PROFILE;
  } else {
    use_profile = profile;
  }

  // The profile directory must be created before anything can be saved in it.
  FileSystem::create_path (use_profile);

  // Save all the open maps.
  for (std::list<Map::Map*>::iterator m = maps.begin(); m != maps.end(); ++m) {
    Map::Map &map = **m;

    if (editor) {
      // If the map was saved in the current profile, delete that obsolete saved
      // copy.
      std::string saved_map_filename = profile + '/' + map.get_name();
      FileSystem::remove (saved_map_filename);
    }

    // Save the map without the player.
    bool map_had_player;
    if (map.get_sprite_by_id ("xeno")) {
      map.detatch_sprite (*player);
      map_had_player = true;
    } else {
      map_had_player = false;
    }
    map.save (use_profile);
    if (map_had_player) {
      map.attach_sprite (*player);
    }
  }

  if (!editor) {
    std::string world_filename = use_profile + "/start.dat";
    std::ofstream f (world_filename.c_str());

    // Save the player.
    f << "create_player " << player->to_string() << std::endl;
    std::string current_map_name;
    if (current_map->get_name().find (' ') == std::string::npos) { // no space
      current_map_name = current_map->get_name();
    } else { // If there's a space in the map name...
      current_map_name = '"' + current_map->get_name() + '"';
    }
    f << "teleport_player " << current_map_name << ' ' <<
        player->get_position().x << ' ' << player->get_position().y << std::endl;

    // Save the current music's filename.
    std::string music_file = music->get_filename().substr (MUSIC_PATH.length());
    f << "play_music " << music_file;

    f.close();
  }
}

void World::start_editor() throw() {
  camera->follow (NULL);
  detatch_sprite (*player);
  player_map_before_editing = current_map->get_name();
  while (!maps.empty()) {
    delete maps.front();
    maps.pop_front();
  }
  current_map = new Map::Map (DEFAULT_PROFILE, player_map_before_editing, *this);
  maps.push_back (current_map);
  editor = new Map::Editor (*this);
  editor->set_map (current_map);
}

void World::execute_commands (unsigned int tick) throw() {
  while (!queued_commands.empty()) {
    std::string command_string;

    // Read the next command.
    std::pair<const unsigned int, std::string> &command =
        *queued_commands.begin();
    unsigned int command_tick = command.first;
    if (tick >= command_tick) {
      command_string = command.second;
      queued_commands.erase (queued_commands.begin());
    }
    if (tick < command_tick) {
      break;
    }

    StringTokenizer tokens (command_string);
    std::string action = tokens.get_next_string();
    if (action == "teleport_player") {
      // Teleport the player to a certain location on some map.

      // Get command parameters.
      std::string map_name = tokens.get_next_string();
      int x = tokens.get_next_integer();
      int y = tokens.get_next_integer();

      teleport_player (map_name, x, y);
    } else if (action == "teleport_player_to_door") {
      // Teleport the player to another map's door.

      // Get command parameters.
      std::string map_name = tokens.get_next_string();
      std::string door_id = tokens.get_next_string();

      teleport_player (map_name, door_id);
    } else if (action == "create_player") {
      // Creates a new avatar to use as the player.
      SpriteLayer::Sprite *sprite =
          Sprite::from_string (tokens.get_remaining_string());
      SpriteLayer::Avatar *avatar = dynamic_cast<SpriteLayer::Avatar*> (sprite);
      if (avatar) {
        // Delete the default AI controller.
        delete avatar->get_controller();
        avatar->pop_controller();

        if (current_map) {
          current_map->attach_sprite (*avatar);
        }
        set_player (avatar);
      } else {
        terminal->display_message ("Unable to create player!");
      }
    } else if (action == "set_player") {
      // Set an existing avatar to be the player.
      std::string id = tokens.get_next_string();
      Sprite *sprite = current_map->get_sprite_by_id (id);
      SpriteLayer::Avatar *avatar = dynamic_cast<SpriteLayer::Avatar*> (sprite);
      if (avatar) {
        set_player (avatar);
      } else {
        terminal->display_message (
            "Unable to set sprite \"" + id + "\" as the player!");
      }
    } else if (action == "heal_player") {
      get_player()->affect_health (get_player()->get_full_health());
    } else if (action == "message_box") {
      message_box = new MessageBox (tokens.get_next_string(),
                                    tokens.get_next_string());

      // Don't execute the next command until this message box is done.
      break;
    } else if (action == "play_music") {
      delete music;
      music = new Audio (MUSIC_PATH + tokens.get_next_string(), true);
      music->play();
    } else if (action == "attach_camera") {
      std::string id = tokens.get_next_string();
      Sprite *sprite = current_map->get_sprite_by_id (id);
      if (sprite) {
        camera->follow (sprite);
      } else {
        terminal->display_message (
            "Unable to follow sprite \"" + id + "\" with the camera!");
      }
    } else if (action == "achievement") {
      delete achievement;
      achievement = new Achievement (tokens.get_next_string());
    } else if (action == "title") {
      delete title;
      title = new Title (tokens.get_next_string());
    } else if (action == "execute") {
      std::ifstream f (tokens.get_next_string().c_str());
      std::string str;

      // Read the number of commands in the file.
      std::getline (f, str);
      StringTokenizer tokens = StringTokenizer (str);
      int command_count = tokens.get_next_integer();

      for (int i = 0; i < command_count; ++i) {
        std::getline (f, str);
        queue_command (str);
      }

      f.close();
    } else if (action == "player_won") {
      // Beat the game.
      status = PLAYER_WON;
    } else if (action == "edit_map") {
      std::string map_name = tokens.get_next_string();
      if (!FileSystem::exists ("maps/" + map_name)) {
        std::ofstream f ("maps/index.dat", std::ios::app);
        f << std::endl << map_name;
        f.close();
      }
      start_editor();
      set_map (map_name);
    } else {
      bool executed = current_map->process_command (command_string);
      if (!executed) {
        terminal->display_message ("Unable to execute the command: "
                                   + command_string);
      }
    } // actions
  } // for each queued command
}

World::World (const std::string &profile) {
  this->profile = profile;

  player = NULL;
  player_status = new AvatarStatus ("player");

  current_map = NULL;
  camera = new Camera (Window::get_width(), Window::get_height());
  music = NULL;
  achievement = NULL;
  title = NULL;

  message_box = NULL;
  message_box_toggling_off = false;

  terminal = new Terminal (*this);

  editor = NULL;
  editor_toggled = false;

  status = RUNNING;

  std::string start_filename = profile + "/start.dat";

  // If the profile's start commands file doesn't exist, use the default.
  std::ifstream test_f (start_filename.c_str());
  if (test_f) { // If the profile contains the requested start commands file...
    test_f.close();
  } else { // If the profile doesn't contain the requested file...
    start_filename = DEFAULT_PROFILE + "/start.dat";
  }

  // Execute the world's starting commands.
  std::ifstream f (start_filename.c_str());
  while (!f.eof()) {
    std::string command;
    std::getline (f, command);
    queue_command (command);
  }
  f.close();
}

World::~World() {
  // If the map editor is in use, save changes and deactivate it.
  if (editor) {
    save();
    delete editor;
  }

  // Delete the player sprite.
  if (current_map) {
    detatch_sprite (*player);
  }
  delete player;

  // Delete the maps.
  for (std::list<Map::Map*>::iterator m = maps.begin(); m != maps.end(); ++m) {
    delete *m;
  }

  delete player_status;
  delete achievement;
  delete title;
  delete message_box; // deleting a NULL pointer has no effect
  delete terminal;
  delete music;
}

void World::set_map (const std::string &map_name,
                     const std::string &profile) throw() {
  // Check to see if the requested map was already loaded.
  current_map = NULL;
  for (std::list<Map::Map*>::iterator m = maps.begin();
       m != maps.end();
       ++m) {
    Map::Map *map = *m;
    if (map->get_name() == map_name) {
      current_map = map;
      break;
    }
  }

  // If the requested map isn't loaded, load it now.
  if (!current_map) {
    std::string use_profile;
    std::string filename = profile + '/' + map_name + "/general.dat";
    std::ifstream f (filename.c_str());
    if (f) { // If the profile contains the requested map...
      f.close();
      use_profile = profile;
    } else { // If the profile doesn't contain the requested map...
      use_profile = DEFAULT_PROFILE;
    }
    current_map = new Map::Map (use_profile, map_name, *this);
    maps.push_back (current_map);
  }

  current_map->set_sprite_id_visibility (terminal->is_visible());

  if (editor) {
    editor->set_map (current_map);
  }
}

void World::set_player (SpriteLayer::Avatar *avatar) throw() {
  // Revert the previous player's controller to what it was before becoming the
  // player.
  if (player) {
    delete player->get_controller();
    player->pop_controller();
  }

  player = avatar;

  // Set the avatar to use the player controller, but remember the previous
  // controller in case this avatar ceases to be the player.
  if (avatar) {
    avatar->push_controller (new Controllers::Player());
  }

  camera->follow (avatar);
  player_status->monitor (avatar);
}

Avatar *World::get_player() const throw() {
  return player;
}

Avatar *World::get_nearest_enemy (const SpriteLayer::Avatar &avatar,
                                           int *distance) const throw() {
  return current_map->get_nearest_enemy (avatar, distance);
}

Sprite *World::get_touching_sprite (int x, int y) const throw() {
  return current_map->get_touching_sprite (x, y);
}

std::list<Sprite*> World::get_touching_sprites (
    const Rectangle &rectangle) const throw() {
  return current_map->get_touching_sprites (rectangle);
}

void World::queue_command (const std::string &command) throw() {
  size_t command_start = 0, command_end;
  do { // for each sub command
    command_end = command.find (';', command_start);
    std::string subcommand = command.substr (command_start,
                                             command_end - command_start);

    if (!subcommand.empty()) {
      int tick;
      std::string string;

      // Get the optional delay of the command.
      StringTokenizer tokens (subcommand);
      if (tokens.get_next_string() == "delay") {
        tick = tokens.get_next_integer() + last_frame_tick;
        string = tokens.get_remaining_string();
      } else {
        tick = 0;
        string = subcommand;
      }

      // Write the command to the queue when not reading commands in another thread.
      std::pair<unsigned int, std::string> command_package =
          std::pair<unsigned int, std::string> (tick, string);
      queued_commands.insert (command_package);
    }

    command_start = command_end + 1;
  } while (command_end != std::string::npos);
}

bool World::is_colliding_with_environment (const Sprite &sprite, bool *fatal)
    const throw() {
  return current_map->is_colliding_with_environment (sprite, fatal);
}

void World::hit_tiles (const Rectangle &rectangle) throw() {
  current_map->get_mainground_tiles()->hit (rectangle);
}

void World::add (Sprite &sprite) throw() {
  current_map->attach_sprite (sprite);
}

void World::quake (int magnitude) throw() {
  current_map->quake (magnitude);
}

void World::teleport_player (const std::string &map_name,
                             int x, int y) throw() {
  // Remove the player from the current map.
  if (current_map) {
    detatch_sprite (*player);
  }

  set_map (map_name, profile);

  // Place the player within the new map.
  player->set_position (x, y);
  current_map->attach_sprite (*player);
}

void World::teleport_player (const std::string &map_name,
                             const std::string &door_id) throw() {
  // Remove the player from the current map.
  if (current_map) {
    detatch_sprite (*player);
  }

  set_map (map_name, profile);

  // Place the player within the new map.
  Door *destination_door = dynamic_cast<Door*> (
      current_map->get_sprite_by_id (door_id));
  if (!destination_door) {
    terminal->display_message (
        "Unable to teleport player to the non-exist door \""
        + door_id + "\" in map \""
        + map_name + "\".");
    return;
  }
  Rectangle door_rect = destination_door->get_position();
  Rectangle player_rect = player->get_position();
  int player_x = door_rect.x + (door_rect.w - player_rect.w) / 2;
  int player_y = door_rect.y;
  player->set_position (player_x, player_y);
  current_map->attach_sprite (*player);
}

void World::update (unsigned int tick) throw() {
  if (last_frame_tick == UNSET) {
    last_frame_tick = tick;
  }

  // Pause and unpause the world.
  if (!terminal->is_visible()) {
    if (Controllers::Manager::is_escape_pressed()) {
      if (status == RUNNING) {
        status = PAUSED;
      } else if (status == PAUSED) {
        status = RUNNING;
      }
    }
  }

  // If not paused, advance the world to a future state.
  if (status == RUNNING) {
    if (editor) {
      editor->update (camera->get_viewport(), tick);

      // Delete all commands that were queued due to the level editor.  For
      // example, we don't want the camera following a boss when switching to a
      // map with a boss in the level editor.
      queued_commands.clear();
    } else {
      // Update the terminal if applicable.
      if (Controllers::Manager::is_terminal_toggled()) {
        terminal->set_visible (!terminal->is_visible());
        current_map->set_sprite_id_visibility (terminal->is_visible());
      }
      terminal->update (tick);

      // Update an achievement acquisition dialog if applicable.
      if (achievement) {
        achievement->update (tick);
        if (achievement->is_done()) {
          delete achievement;
          achievement = NULL;
        }
      }

      // Update the title if applicable.
      if (title) {
        title->update (tick);
        if (title->is_done()) {
          delete title;
          title = NULL;
        }
      }

      // We want to allow the user to focus on one thing at a time.  That may be
      // interacting with the world or reading a message dialog, but not both.
      if (message_box) {
        message_box->update (tick);

        // Don't get rid of the message box until the user releases the proceed
        // key.  This prevents the proceed key from interfering with the world
        // after the message box is gone.
        static Controllers::Player pc;
        if (pc.is_jump_pressed()) {
          message_box_toggling_off = true;
        } else if (message_box_toggling_off) {
          // Get rid of the current message box.
          delete message_box;
          message_box = NULL; // delete doesn't set the pointer to NULL
          message_box_toggling_off = false;

          execute_commands (tick); // if there's another message box command
                                   // queued, execute it before updating the map
        }
      } else { // If the message box isn't in use...
        if (!terminal->is_visible()) {
          // If the frame rate is too low, the duration between frames is too
          // high.  That would cause sprites to move further and perhaps through
          // small amounts of walls and floors.  To prevent that, we limit the
          // elapsed ticks as they are applied to the world tick count.
          static const unsigned int MAX_TICKS = 50; // in ms
          unsigned int elapsed_ticks = tick - last_frame_tick; // ms since prev frame
          if (elapsed_ticks > MAX_TICKS) {
            elapsed_ticks = MAX_TICKS;
          }
          world_tick += elapsed_ticks;

          if (current_map) {
            current_map->update (*this, world_tick);
          }
          if (player) {
            player_status->update (world_tick);
          }
        } // if the terminal is closed

        execute_commands (tick); // execute queued commands at the end of the
                                 // frame they're queued on
        if (!player) {
          status = PLAYER_LOST;
        }
      } // if the message box isn't in use
      camera->update();
    }

    // Toggle the map editor.
    if (!terminal->is_visible()) {
      Controllers::Editor *editor_controller =
          Controllers::Manager::get_editor_controller();
      if (editor_controller->is_toggle_pressed()) {
        if (!editor_toggled) {
          if (editor) {
            save();

            // Close the editor.
            delete editor;
            editor = NULL; // delete doesn't automatically set it to NULL

            // Place the player in the map.
            if (player) {
              teleport_player (player_map_before_editing,
                               player->get_position().x,
                               player->get_position().y);
              camera->follow (player);
            }
          } else {
            start_editor();
          }
          editor_toggled = true;
        }
      } else {
        editor_toggled = false;
      }
    } // if terminal is closed
  }

  last_frame_tick = tick;
}

void World::detatch_sprite (Sprite &sprite) {
  current_map->detatch_sprite (sprite);
}

void World::draw() throw() {
  if (current_map) {
    camera->translate_view();

    current_map->draw (camera->get_viewport());

    if (editor) {
      editor->draw();
    } else {
      if (message_box) {
        message_box->draw();
      }
      if (terminal->is_visible()) {
        terminal->draw();
      } else {
        player_status->draw();
        if (achievement) {
          achievement->draw();
        }
      }
      if (title) {
        title->draw();
      }
    }
  }
}

void World::resume() throw() {
  last_frame_tick = UNSET;
  status = RUNNING;
}
