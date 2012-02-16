/*
 * MenuItem.cpp
 *
 *  Created on: 2012-01-05
 *      Author: samuel
 */

#include "MenuItem.h"

using Game::UI::MenuItem;

MenuItem::MenuItem() throw() {
  highlighted = false;

  selected = false;
  select_callback = NULL;
  select_callback_data = NULL;
}

MenuItem::~MenuItem() throw() {}

void MenuItem::set_selected (bool selected) throw() {
  this->selected = selected;

  // If an item was selected, as opposed to unselected...
  if (selected) {
    // Call a callback if one is registered.
    if (select_callback) {
      select_callback (select_callback_data);
    }
  }
}

void MenuItem::set_select_callback (
    void (*select_callback) (void *select_callback_data) throw(),
    void *select_callback_data) throw() {
  this->select_callback = select_callback;
  this->select_callback_data = select_callback_data;
}

Game::UI::Widget *MenuItem::mouse_move (int x, int y) throw() {
  set_highlighted (true);
  return this;
}

void MenuItem::mouse_primary_pressed (int x, int y) throw() {
  set_selected (true);
}
