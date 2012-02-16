/*
 * Layout.cpp
 *
 *  Created on: 2012-01-14
 *      Author: samuel
 */

#include <cassert>
#include <cstring> // for NULL
#include <algorithm> // for std::find
#include <GL/gl.h>
#include "Manager.h"
#include "Layout.h"

using Game::UI::Layout;

Layout::Layout() throw() {
  pressed_widget = NULL;
}

Layout::~Layout() throw() {
  // Delete consumed child widgets.
  for (std::vector<Widget*>::iterator w = child_widgets.begin();
       w != child_widgets.end();
       ++w) {
    delete *w;
  }
}

void Layout::consume (Widget *widget) throw() {
  widget->set_parent (this);
  child_widgets.push_back (widget);
}

void Layout::unconsume (Widget *widget) throw() {
  // Remove the widget from this layout.
  std::vector<Widget*>::iterator w =
      std::find (child_widgets.begin(), child_widgets.end(), widget);
  assert (w != child_widgets.end());
  child_widgets.erase (w);

  // Make the manager forget about the unconsumed subwidget in its hierarchy.
  Widget *pw = widget;
  for (;;) {
    Widget *p = pw->get_parent();
    if (p) {
      pw = p;
    } else {
      break;
    }
  }
  Manager *manager = pw->get_manager();
  if (manager) {
    manager->forget_widget (widget);
  }

  widget->set_parent (NULL);
}

Game::UI::Widget *Layout::get_widget (int index) const throw() {
  assert (index >= 0 && index < get_child_count());
  return child_widgets[index];
}

Game::UI::Widget *Layout::mouse_move (int x, int y) throw() {
  int x_relative_to_grid = x - location.x;
  int y_relative_to_grid = y - location.y;
  for (std::vector<Widget*>::iterator w = child_widgets.begin();
       w != child_widgets.end();
       ++w) {
    Widget &widget = **w;
    if (widget.get_position().intersects (x_relative_to_grid,
                                          y_relative_to_grid)) {
      Widget *hovered_widget =
          widget.mouse_move (x_relative_to_grid, y_relative_to_grid);
      return hovered_widget;
    }
  }
  return this; // no child widget is hovered over so this widget is hovered over
}

void Layout::mouse_primary_pressed (int x, int y) throw() {
  int x_relative_to_grid = x - location.x;
  int y_relative_to_grid = y - location.y;
  for (std::vector<Widget*>::iterator w = child_widgets.begin();
       w != child_widgets.end();
       ++w) {
    Widget &widget = **w;
    if (widget.get_position().intersects (x_relative_to_grid,
                                          y_relative_to_grid)) {
      widget.mouse_primary_pressed (x_relative_to_grid, y_relative_to_grid);
      pressed_widget = &widget;
      break;
    }
  }
}

void Layout::mouse_primary_released (int x, int y) throw() {
  if (pressed_widget) {
    int x_relative_to_grid = x - location.x;
    int y_relative_to_grid = y - location.y;
    pressed_widget->mouse_primary_released (x_relative_to_grid,
                                            y_relative_to_grid);
    pressed_widget = NULL;
  }
}

void Layout::mouse_exit() throw() {
  // TODO
}

void Layout::update (unsigned int tick) throw() {
  for (std::vector<Widget*>::iterator w = child_widgets.begin();
       w != child_widgets.end();
       ++w) {
    (*w)->update (tick);
  }
}

void Layout::draw() const throw() {
  // Translate to the grid's location.
  glPushMatrix(); // save the position before drawing
  glTranslatef (location.x, location.y, 0.0F);

  // Draw the items.
  for (std::vector<Widget*>::const_iterator w = child_widgets.begin();
       w != child_widgets.end();
       ++w) {
    (*w)->draw();
  }

  glPopMatrix(); // return to the position from before drawing
}
