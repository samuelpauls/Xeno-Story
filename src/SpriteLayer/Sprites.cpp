/*
 * Sprites.cpp
 *
 *  Created on: 2011-08-15
 *      Author: samuel
 */

#include <cmath>
#include <climits> // has INT_MAX
#include <fstream>
#include "../World.h"
#include "../StringTokenizer.h"
#include "Rock.h"
#include "Projectile.h"
#include "Torch.h"
#include "Door.h"
#include "Sprites.h"

using Game::SpriteLayer::Sprites;
using Game::SpriteLayer::Sprite;
using Game::SpriteLayer::Avatar;

Game::Font Sprites::id_text (10);

Sprites::Sprites (const std::string &map_path) throw(): Map::Layer ("Sprites") {
  id_visibility = false;
  updating = false;

  // Process the sprites data file if it exists.
  std::string filename = map_path + "/sprites.dat";
  std::ifstream f (filename.c_str());
  if (f) {
    // Process each sprite in the file.
    while (!f.eof()) {
      std::string sprite_string;
      std::getline (f, sprite_string);
      if (sprite_string != "") {
        attach (Sprite::from_string (sprite_string));
      }
    }

    f.close();
  }
}

Sprites::~Sprites() throw() {
  // Delete all the sprites.
  for (std::list<Sprite*>::iterator i = sprites.begin();
       i != sprites.end();
       ++i) {
    delete *i;
  }
}

bool Sprites::process_command (const std::string &command) throw() {
  if (!command.empty()) {
    StringTokenizer tokens (command);
    std::string action = tokens.get_next_string();
    if (action == "add_sprite") {
      attach (Sprite::from_string (tokens.get_remaining_string()));
      return true;
    } else if (action == "close_all_doors") {
      for (std::list<Sprite*>::const_iterator s = sprites.begin();
           s != sprites.end();
           ++s) {
        Door *door = dynamic_cast<Door*> (*s);
        if (door) {
          door->set_open (false);
        }
      } // for each sprite
      return true;
    } else if (action == "open_all_doors") {
      for (std::list<Sprite*>::const_iterator s = sprites.begin();
           s != sprites.end();
           ++s) {
        Door *door = dynamic_cast<Door*> (*s);
        if (door) {
          door->set_open (true);
        }
      } // for each sprite
      return true;
    } else {
      for (std::list<Sprite*>::const_iterator s = sprites.begin();
           s != sprites.end();
           ++s) {
        Sprite &sprite = **s;
        if (sprite.get_id() == action.substr(0, sprite.get_id().length())) {
          return sprite.execute (command);
          break;
        }
      }
    } // if a particular action
  }

  return false;
}

void Sprites::attach (Sprite *sprite) throw() {
  sprites.push_back (sprite);
  sprite->play_spawn_sound();
}

void Sprites::detatch (Sprite &sprite) throw() {
  if (updating) {
    if (&sprite == *iterator) {
      ++iterator;
      iterator_incremented = true;
    }
    if (&sprite == *iterator_b) {
      ++iterator_b;
      iterator_b_incremented = true;
    }
  }
  sprites.remove (&sprite);
}

Sprite *Sprites::get_by_id (const std::string &id) const throw() {
  for (std::list<Sprite*>::const_iterator s = sprites.begin();
       s != sprites.end();
       ++s) {
    Sprite *sprite = *s;
    if (sprite->get_id() == id) {
      return sprite;
    }
  }

  return NULL; // no sprite with that ID found
}

Avatar *Sprites::get_nearest_enemy (
    const Avatar &avatar, int *distance) const throw() {
  int nearest_enemy_distance = INT_MAX;
  Avatar *nearest_enemy = NULL;

  for (std::list<Sprite*>::const_iterator s = sprites.begin();
       s != sprites.end();
       ++s) {
    if (Avatar *another_avatar = dynamic_cast<Avatar*> (*s)) {
      if (another_avatar->get_team() != avatar.get_team()) {
        // Calculate the distance from the given avatar to an enemy.
        int ax = avatar.get_position().x;
        int ay = avatar.get_position().y;
        int a2x = another_avatar->get_position().x;
        int a2y = another_avatar->get_position().y;
        int dx = ax - a2x;
        int dy = ay - a2y;
        int d = std::sqrt (dx * dx + dy * dy);

        // Remember this distance if it's the closest so far.
        if (d < nearest_enemy_distance) {
          nearest_enemy = another_avatar;
          nearest_enemy_distance = d;
        }
      } // if the teams are different
    } // if another sprite is an avatar and an enemy
  } // for each sprite

  // If the user provided a distance pointer, store the distance that was
  // already found as a byproduct of the search for the nearest enemy.
  if (distance) {
    *distance = nearest_enemy_distance;
  }

  return nearest_enemy;
}

std::list<Sprite*> Sprites::get_touching_sprites (const Sprite &sprite)
                                                 const throw() {
  std::list<Sprite*> touching_sprites;

  for (std::list<Sprite*>::const_iterator s = sprites.begin();
       s != sprites.end();
       ++s) {
    if ((*s)->get_position().intersects (sprite.get_position())) {
      if (*s != &sprite) { // if the touching sprites are not the same sprite
        touching_sprites.push_back (*s);
      }
    } // if the sprites are touching
  } // for each sprite

  return touching_sprites;
}

Sprite *Sprites::get_touching_sprite (int x, int y) const throw() {
  for (std::list<Sprite*>::const_reverse_iterator s = sprites.rbegin();
       s != sprites.rend();
       ++s) {
    if ((*s)->get_position().intersects (x, y)) {
      return *s;
    } // if the sprite is touching
  } // for each sprite

  return NULL; // no touching sprite found
}

std::list<Sprite*> Sprites::get_touching_sprites (
    const Rectangle &rectangle) const throw() {
  std::list<Sprite*> r;

  for (std::list<Sprite*>::const_iterator s = sprites.begin();
       s != sprites.end();
       ++s) {
    if ((*s)->get_position().intersects (rectangle)) {
      r.push_back (*s);
    } // if the sprite is touching
  } // for each sprite

  return r; // no touching sprite found
}

void Sprites::update (World &world, unsigned int tick) throw() {
  updating = true;

  // Update all sprites.
  for (iterator = sprites.begin(); iterator != sprites.end();) {
    iterator_incremented = false;

    Sprite *sprite = *iterator;

    if (sprite->is_dead()) {
      // Get rid of the sprite.
      iterator = sprites.erase (iterator);
      if (sprite == world.get_player()) {
        world.set_player (NULL);
      }
      delete sprite;
    } else {
      sprite->update (&world, tick);

      // If the sprite is now dead, the soonest it will be removed is during the
      // next updating of all sprites.  Even if dead, it may have moved and need
      // to affect other touching sprites during this round of updates.
    }

    if (!iterator_incremented) {
      ++iterator;
    }
  }

  // Now that all sprites are updated, they all get a fair chance at affecting
  // each other.
  for (iterator = sprites.begin(); iterator != sprites.end();) {
    iterator_incremented = false;

    const Rectangle &i_pos = (*iterator)->get_position();

    iterator_b = iterator;
    ++iterator_b;
    while (iterator_b != sprites.end()) {
      iterator_b_incremented = false;

      const Rectangle &j_pos = (*iterator_b)->get_position();

      // If the sprites are touching, they affect each other.
      if (i_pos.intersects (j_pos)) {
        (*iterator)->affect (world, **iterator_b);
        if (iterator != sprites.end() && iterator_b != sprites.end()) {
          (*iterator_b)->affect (world, **iterator);
        }
      } // if sprites touching

      if (!iterator_b_incremented) {
        ++iterator_b;
      }
    } // iterator_b

    if (!iterator_incremented) {
      ++iterator;
    }
  } // iterator

  updating = false;
}

void Sprites::draw (Lighting &lighting, const Rectangle &viewport) throw() {
  for (std::list<Sprite*>::const_iterator s = sprites.begin();
       s != sprites.end();
       ++s) {
    Sprite &sprite = **s;

    // Translate to the sprite.
    glPushMatrix();
    glTranslatef (sprite.get_position().x, sprite.get_position().y, 0.0F);

    sprite.draw (&lighting);

    // Draw the sprite's IDs if requested.
    if (id_visibility) {
      glColor3f (1.0F, 1.0F, 1.0F); // white
      glTranslatef (0.0F, sprite.get_position().h, 0.0F);
      id_text.draw (sprite.get_id());
    }

    glPopMatrix();
  }
}

void Sprites::save (const std::string &map_path) throw() {
  // Open this sprite layer's data file for writing.
  std::string filename = map_path + "/sprites.dat";
  std::ofstream f;
  f.open (filename.c_str());

  // Write each sprite to the file.
  for (std::list<Sprite*>::const_iterator s = sprites.begin();
       s != sprites.end();
       ++s) {
    if (s != sprites.begin()) {
      f << std::endl;
    }
    f << (*s)->to_string();
  }

  f.close();
}
