/*
 * Sprite.cpp
 *
 *  Created on: 2011-08-15
 *      Author: samuel
 */

#include <sstream> // for std::stringstream
#include <fstream>
#include <cstring> // for NULL
#include <cassert>
#include <typeinfo> // has typeid
#include <list>
#include "../StringTokenizer.h"
#include "../Scripting/String.h"
#include "../Scripting/Integer.h"
#include "Sprite.h"

// Needed for the various types of sprites that can be created.
#include "BigAvatar.h"
#include "Door.h"
#include "Elevator.h"
#include "Explosion.h"
#include "Gate.h"
#include "PowerUp.h"
#include "Projectile.h"
#include "Rock.h"
#include "SavePoint.h"
#include "SmallAvatar.h"
#include "Torch.h"
#include "Weapon.h"

using Game::SpriteLayer::Sprite;

void Sprite::set_unique_id() throw() {
  static unsigned int next_id = 0;
  std::stringstream next_id_ss;
  next_id_ss << next_id;
  id = next_id_ss.str();
  ++next_id;
}

void Sprite::register_properties() throw() {
  object.consume_variable (new Scripting::String ("ID", id));
  object.consume_variable (new Scripting::Integer ("X", destination_rectangle->x));
  object.consume_variable (new Scripting::Integer ("Y", destination_rectangle->y));
}

Sprite::Sprite (const std::string &subtype) throw(): object (*this) {
  common = new Common();
  common->reference_count = 1;

  common->subtype = subtype;
  set_unique_id();
  destination_rectangle = new Rectangle();
  dead = false;

  // If there's a spawn sound for this weapon, load it.
  std::string spawn_sound_filename = "sprites/" + subtype + "/spawn.ogg";
  std::ifstream spawn_sound_file (spawn_sound_filename.c_str());
  if (spawn_sound_file) {
    spawn_sound_file.close();
    common->spawn_sound = new Audio (spawn_sound_filename);
  } else {
    common->spawn_sound = NULL;
  }

  register_properties();
}

Sprite::Sprite (const Sprite &sprite) throw(): object (*this) {
  common = sprite.common;
  common->reference_count++;

  set_unique_id();

  dead = sprite.dead;
  destination_rectangle = new Rectangle (*sprite.destination_rectangle);

  register_properties();
}

Sprite::~Sprite() {
  delete destination_rectangle;

  // Delete data that isn't shared with any other cloned sprite.
  if (--common->reference_count == 0) {
    delete common->spawn_sound;
    delete common;
  }
}

Sprite *Sprite::create (const std::string &subtype) throw() {
  Sprite *new_sprite = NULL;

  // One of each sprite is cached upon first load so that we don't have to
  // slowly load it from disk a second time.
  static std::list<Sprite*> cached_sprites;
  for (std::list<Sprite*>::const_iterator s = cached_sprites.begin();
       s != cached_sprites.end();
       ++s) {
    Sprite &cached_sprite = **s;
    if (cached_sprite.get_subtype() == subtype) {
      new_sprite = cached_sprite.clone();
    }
  }

  // Otherwise, if the sprite hasn't been loaded, load and cache it now.
  if (!new_sprite) {
    std::string type; // eg rock

    // Load this sprite's type.
    std::string type_filename = "sprites/" + subtype + "/type.dat";
    std::ifstream f (type_filename.c_str());
    f >> type;
    f.close();

    // Load the sprite for the first time.
    if (type == "BigAvatar") {
      new_sprite = new BigAvatar (subtype);
    } else if (type == "Door") {
      new_sprite = new Door (subtype);
    } else if (type == "Elevator") {
      new_sprite = new Elevator (subtype);
    } else if (type == "Explosion") {
      new_sprite = new Explosion (subtype);
    } else if (type == "Gate") {
      new_sprite = new Gate (subtype);
    } else if (type == "PowerUp") {
      new_sprite = new PowerUp (subtype);
    } else if (type == "Projectile") {
      new_sprite = new Projectile (subtype);
    } else if (type == "Rock") {
      new_sprite = new Rock (subtype);
    } else if (type == "SavePoint") {
      new_sprite = new SavePoint (subtype);
    } else if (type == "SmallAvatar") {
      new_sprite = new SmallAvatar (subtype);
    } else if (type == "Torch") {
      new_sprite = new Torch (subtype);
    } else if (type == "Weapon") {
      new_sprite = new Weapon (subtype);
    } else {
      assert (false); // unknown type of sprite
    }

    // Cache the new sprite for future requests.
    cached_sprites.push_back (new_sprite->clone());
  }

  return new_sprite;
}

void Sprite::play_spawn_sound() throw() {
  if (common->spawn_sound) {
    common->spawn_sound->play();
  }
}

void Sprite::set_position (int x, int y) throw() {
  destination_rectangle->x = x;
  destination_rectangle->y = y;
}

std::string Sprite::to_string() const throw() {
  std::string r;

  // Convert the subtype to a string.
  if (common->subtype.find (' ') == std::string::npos) {
    // If there are no spaces in the subtype...
    r += common->subtype;
  } else {
    // If there are spaces in the subtype...
    r += '"' + common->subtype + '"';
  }

  r += ' ' + object.to_string();

  return r;
}

Sprite *Sprite::from_string (const std::string &string) throw() {
  StringTokenizer tokens (string);
  std::string subtype = tokens.get_next_string(); //eg door as opposed to rock
  Sprite *sprite = Sprite::create (subtype);
  sprite->object.initialize (tokens);
  return sprite;
}

bool Sprite::execute (const std::string &command) throw() {
  int name_start = command.find ('.') + 1;
  int name_length = command.find (' ') - name_start;
  std::string name = command.substr (name_start, name_length);
  int value_start = command.find ('=') + 2; // skip space too
  int value_length = command.length() - value_start;
  std::string value = command.substr (value_start, value_length);
  return object.set_variable (name, value);
}
