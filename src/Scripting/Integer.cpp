/*
 * Integer.cpp
 *
 *  Created on: 2012-01-10
 *      Author: samuel
 */

#include <cassert>
#include <cstdlib> // for std::atoi
#include <sstream>
#include "Integer.h"

using Game::Scripting::Integer;

void Integer::set_from_widget (void *property) throw() {
  Integer *casted = (Integer*)property;
  casted->set_value (casted->text_box->get_text());
}

Integer::Integer (const std::string &human_name, int &value) throw():
    Variable (human_name), value (value) {
  text_box = NULL;
}

Integer::~Integer() throw() {
  assert (!text_box);
}

bool Integer::set_value (const std::string &value) throw() {
  int tentative_value = std::atoi (value.c_str());
  if (tentative_value == 0) { // could be due to invalid number
    for (int v = 0; v < value.length(); ++v) {
      char c = value[v];
      if (c != '0') {
        return false; // return failure
      }
    }
  }
  this->value = tentative_value;
  Variable::set_value (value);
  return true; // return success
}

std::string Integer::get_value() const throw() {
  std::stringstream ss;
  ss << value;
  return ss.str();
}

Game::UI::Widget *Integer::create_value_widget() throw() {
  assert (!text_box);
  text_box = new UI::TextBox();
  update_value_widget();
  text_box->set_enter_callback (set_from_widget, this);
  return text_box;
}

void Integer::update_value_widget() throw() {
  assert (text_box);
  text_box->set_text (get_value());
}

void Integer::destroy_value_widget() throw() {
  assert (text_box);
  delete text_box;
  text_box = NULL;
}
