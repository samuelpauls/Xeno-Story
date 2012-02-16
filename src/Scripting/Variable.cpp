/*
 * Variable.cpp
 *
 *  Created on: 2012-01-10
 *      Author: samuel
 */

#include <cctype> // for std::tolower
#include "Object.h"
#include "Variable.h"

using Game::Scripting::Variable;

Variable::Variable (const std::string &human_name) throw() {
  this->human_name = human_name;

  // Create a machine name from the human name.
  machine_name = human_name;
  for (unsigned int i = 0; i < human_name.length(); ++i) {
    int c = human_name[i];
    if (c == ' ') {
      c = '_';
    } else if (std::isalpha (c)) {
      c = std::tolower (c);
    }
    machine_name[i] = c;
  }

  object = NULL;
}

Variable::~Variable() throw() {}

bool Variable::set_value (const std::string &value) throw() {
  object->on_variable_changed (get_human_name());
  return true; // success
}
