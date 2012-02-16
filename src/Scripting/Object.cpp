/*
 * Object.cpp
 *
 *  Created on: 2012-01-11
 *      Author: samuel
 */

#include <cassert>
#include <typeinfo> // for typeid
#include "../UI/Label.h"
#include "Object.h"

using Game::Scripting::Object;

Object::Object() {
  properties_widget = NULL;
}

Object::~Object() {
  assert (!properties_widget);

  // Delete all added variables.
  for (std::vector<Variable*>::const_iterator v = variables.begin();
       v != variables.end();
       ++v) {
    delete *v;
  }
}

std::string Object::to_string() const throw() {
  std::string r;
  for (std::vector<Variable*>::const_iterator v = variables.begin();
       v != variables.end();
       ++v) {
    Variable &var = **v;
    std::string val = var.get_value();
    if (!val.empty()) {
      if (v != variables.begin()) {
        r += ' ';
      }

      r += var.get_machine_name();

      r += ':';

      if (val.find (' ') == std::string::npos) {
        // If the value has no spaces...
        r += val;
      } else {
        // If the value has spaces...
        r += '"' + val + '"';
      }
    }
  }
  return r;
}

void Object::initialize (StringTokenizer &tokens) throw() {
  while (tokens.has_more_tokens()) {
    tokens.set_delimiter (':');
    std::string name = tokens.get_next_string();
    tokens.set_delimiter (' ');
    std::string value = tokens.get_next_string();
    for (std::vector<Variable*>::const_iterator v = variables.begin();
         v != variables.end();
         ++v) {
      Variable &var = **v;
      if (var.get_machine_name() == name) {
        var.set_value (value);
        break; // break out of for loop
      }
    }
  }
}

void Object::consume_variable (Variable *variable) {
  variables.push_back (variable);
  variable->set_parent_object (this);
}

bool Object::set_variable (const std::string &name, const std::string &value)
                          throw() {
  for (std::vector<Variable*>::const_iterator v = variables.begin();
       v != variables.end();
       ++v) {
    Variable &var = **v;
    if (name == var.get_machine_name()) {
      return var.set_value (value);
    }
  }
  return false; // failed because variable not found
}

Game::UI::Widget *Object::create_properties_widget() throw() {
  assert (!properties_widget);
  static const int ITEM_PADDING = 4; // in pixels
  static const int MAXIMUM_WIDGET_HEIGHT = 20; // in pixels
  properties_widget = new UI::GridLayout (
                              2, variables.size(),
                              ITEM_PADDING,
                              UI::GridLayout::UNLIMITED, MAXIMUM_WIDGET_HEIGHT);
  for (unsigned int v = 0; v < variables.size(); ++v) {
    Scripting::Variable &variable = *variables[v];
    UI::Label *label = new UI::Label (variable.get_human_name() + ": ");
    properties_widget->consume (0, v, label);
    properties_widget->consume (1, v, variable.create_value_widget());
  }
  return properties_widget;
}

void Object::update_properties_widget() throw() {
  for (std::vector<Variable*>::const_iterator v = variables.begin();
       v != variables.end();
       ++v) {
    (*v)->update_value_widget();
  }
}

void Object::destroy_properties_widget() throw() {
  assert (properties_widget);

  // Delete all non-label widgets from the properties layout.
  for (unsigned int r = 0; r < variables.size(); ++r) {
    properties_widget->unconsume (1, r);
    variables[r]->destroy_value_widget();
  }

  delete properties_widget;
  properties_widget = NULL;
}
