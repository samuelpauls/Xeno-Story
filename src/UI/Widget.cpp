/*
 * Widget.cpp
 *
 *  Created on: 2011-12-28
 *      Author: samuel
 */

#include <cstring> // for NULL
#include "Widget.h"

using Game::UI::Widget;

void Widget::get_position_in_window (int &x, int &y) const throw() {
  x += location.x;
  y += location.y;
  if (parent) {
    parent->get_position_in_window (x, y);
  }
}

Game::Rectangle Widget::get_position_in_window() const throw() {
  Rectangle loc_in_win;
  get_position_in_window (loc_in_win.x, loc_in_win.y);
  loc_in_win.w = location.w;
  loc_in_win.h = location.h;
  return loc_in_win;
}

Widget::Widget() throw() {
  manager = NULL;
  parent = NULL;
}

Widget::~Widget() throw() {}

bool Widget::intersects (int window_x, int window_y) const throw() {
  return get_position_in_window().intersects (window_x, window_y);
}
