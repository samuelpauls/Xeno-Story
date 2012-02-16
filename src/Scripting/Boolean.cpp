/*
 * Boolean.cpp
 *
 *  Created on: 2012-01-11
 *      Author: samuel
 */

#include <cassert>
#include "Boolean.h"

using Game::Scripting::Boolean;

void Boolean::set_from_widget (void *property) throw() {
  Boolean *casted = (Boolean*)property;
  casted->value = casted->toggle->get_value();
  casted->set_value (casted->get_value()); // call on variable changed
}

Boolean::Boolean (const std::string &human_name, bool &value) throw():
    Variable (human_name), value (value) {
  toggle = NULL;
}

Boolean::~Boolean() throw() {
  assert (!toggle);
}

bool Boolean::set_value (const std::string &value) throw() {
  if (value == "1") {
    this->value = true;
  } else if (value == "0") {
    this->value = false;
  } else {
    return false; // return failure
  }
  Variable::set_value (value);
  return true; // return success
}

std::string Boolean::get_value() const throw() {
  if (value) {
    return "1";
  } else {
    return "0";
  }
}

Game::UI::Widget *Boolean::create_value_widget() throw() {
  assert (!toggle);
  toggle = new UI::Toggle (value);
  toggle->set_value_changed_callback (set_from_widget, this);
  return toggle;
}

void Boolean::update_value_widget() throw() {
  assert (toggle);
  toggle->set_value (value);
}

void Boolean::destroy_value_widget() throw() {
  assert (toggle);
  delete toggle;
  toggle = NULL;
}
