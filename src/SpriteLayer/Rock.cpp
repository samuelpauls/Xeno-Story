/*
 * Rock.cpp
 *
 *  Created on: 2011-08-15
 *      Author: samuel
 */

#include <fstream>
#include "../Scripting/String.h"
#include "../Animation.h"
#include "Rock.h"
#include "Avatar.h"

using Game::SpriteLayer::Rock;

void Rock::register_properties() throw() {
  object.consume_variable (new Scripting::String ("Unique Interaction Command", unique_interaction_command));
}

Rock::Rock (const std::string &subtype) throw(): SpriteLayer::Sprite (subtype) {
  animation = new Animation ("sprites/" + subtype + "/animation.png");
  destination_rectangle->w = animation->get_width();
  destination_rectangle->h = animation->get_height();

  // Attempt to load player interaction commands from a file in the sprite's
  // directory.
  std::string common_interactions_filename =
      "sprites/" + subtype + "/player_interaction.dat";
  std::ifstream f (common_interactions_filename.c_str());
  if (f) {
    while (!f.eof()) {
      std::string command;
      std::getline(f, command);
      common_interaction_commands.push_back (command);
    }
    f.close();
  }

  register_properties();
}

Rock::Rock (const Rock &rock) throw(): Sprite (rock) {
  animation = new Animation (*rock.animation);
  unique_interaction_command = rock.unique_interaction_command;
  common_interaction_commands = rock.common_interaction_commands;

  register_properties();
}

Rock::~Rock() throw() {
  delete animation;
}

void Rock::update (World *world, unsigned int tick) throw() {
  animation->update (tick);
}

void Rock::affect (World &world, Sprite &sprite) throw() {
  if (unique_interaction_command != "" ||
      !common_interaction_commands.empty()) {
    if (&sprite == world.get_player()) {
      if (world.get_player()->get_controller()->is_down_pressed()) {
        // Execute the player interaction commands that were loaded from the
        // sprite's directory.
        for (std::list<std::string>::iterator i =
                 common_interaction_commands.begin();
             i != common_interaction_commands.end();
             ++i) {
          world.queue_command (*i);
        }

        world.queue_command (unique_interaction_command);
      }
    }
  }
}

void Rock::draw (Lighting *lighting) throw() {
  animation->draw();
}
