/*
 * Test.cpp
 *
 *  Created on: 2011-08-30
 *      Author: samuel
 */

#include <cassert>
#include "StringTokenizer.h"
#include "Test.h"

void Game::testStringTokenizer() throw() {
  StringTokenizer tokens ("add_sprite Fireball "
      "weapon_start_x weapon_start_y angle=1.57 weapon_team");
  assert (tokens.get_next_string() == "add_sprite");
  assert (tokens.get_next_string() == "Fireball");
  assert (tokens.get_next_string() == "weapon_start_x");
  assert (tokens.get_next_string() == "weapon_start_y");
  assert (tokens.get_next_string() == "angle=1.57");
  assert (tokens.has_more_tokens());
  assert (tokens.get_next_string() == "weapon_team");
  assert (!tokens.has_more_tokens());

  StringTokenizer tokens2 ("add_sprite Door 213 200 \"teleport_player Outside 300 200\"");
  assert (tokens2.get_next_string() == "add_sprite");
  assert (tokens2.get_next_string() == "Door");
  assert (tokens2.get_next_string() == "213");
  assert (tokens2.get_next_string() == "200");
  assert (tokens2.get_next_string() == "teleport_player Outside 300 200");
}
