/*
 * String.cpp
 *
 *  Created on: 2012-01-10
 *      Author: samuel
 */

#include <cassert>
#include "String.h"

using Game::Scripting::String;

void String::set_from_widget (void *property) throw() {
  String *casted = (String*)property;
  casted->set_value (casted->text_box->get_text());
}

String::String (const std::string &human_name, std::string &value) throw():
    Variable (human_name), value (value) {
  text_box = NULL;
}

String::~String() throw() {
  assert (!text_box);
}

bool String::set_value (const std::string &value) throw() {
  this->value = value;
  Variable::set_value (value);
  return true; // return success
}

Game::UI::Widget *String::create_value_widget() throw() {
  assert (!text_box);
  text_box = new UI::TextBox();
  update_value_widget();
  text_box->set_enter_callback (set_from_widget, this);
  return text_box;
}

void String::update_value_widget() throw() {
  assert (text_box);
  text_box->set_text (value);
}

void String::destroy_value_widget() throw() {
  assert (text_box);
  delete text_box;
  text_box = NULL;
}
