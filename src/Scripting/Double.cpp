/*
 * Double.cpp
 *
 *  Created on: 2012-01-17
 *      Author: samuel
 */

#include <cassert>
#include <cstdlib> // for std::atof
#include <sstream>
#include "Double.h"

using Game::Scripting::Double;

void Double::set_from_widget (void *property) throw() {
  Double *casted = (Double*)property;
  casted->set_value (casted->text_box->get_text());
}

Double::Double (const std::string &human_name, double &value) throw():
    Variable (human_name), value (value) {
  text_box = NULL;
}

Double::~Double() throw() {
  assert (!text_box);
}

bool Double::set_value (const std::string &value) throw() {
  double tentative_value = std::atof (value.c_str());
  if (tentative_value == 0.0) { // could be due to invalid number
    for (int v = 0; v < value.length(); ++v) {
      char c = value[v];
      if (c != '0' && c != '.') {
        return false; // return failure
      }
    }
  }
  this->value = tentative_value;
  Variable::set_value (value);
  return true; // return success
}

std::string Double::get_value() const throw() {
  std::stringstream ss;
  ss << value;
  return ss.str();
}

Game::UI::Widget *Double::create_value_widget() throw() {
  assert (!text_box);
  text_box = new UI::TextBox();
  update_value_widget();
  text_box->set_enter_callback (set_from_widget, this);
  return text_box;
}

void Double::update_value_widget() throw() {
  assert (text_box);
  text_box->set_text (get_value());
}

void Double::destroy_value_widget() throw() {
  assert (text_box);
  delete text_box;
  text_box = NULL;
}
