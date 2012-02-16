/*
 * PaletteLayout.cpp
 *
 *  Created on: 2012-01-03
 *      Author: samuel
 */

#include <cassert>
#include <algorithm> // for std::max
#include <GL/gl.h>
#include "PaletteLayout.h"

using Game::UI::PaletteLayout;

Game::Rectangle PaletteLayout::calculate_widget_position (int index)
                               const throw() {
  Rectangle wid_pos;
  int widgets_in_row =
      std::max (1, (location.w - 2 * item_padding) / (item_width + item_padding));
  int x_index = index % widgets_in_row;
  int y_index = index / widgets_in_row;
  wid_pos.x = item_padding + x_index * (item_width + item_padding);
  wid_pos.y = location.h - (y_index + 1) * (item_height + item_padding);
  wid_pos.w = item_width;
  wid_pos.h = item_height;
  return wid_pos;
}

PaletteLayout::PaletteLayout (int item_width, int item_height,
                              int item_padding) throw() {
  this->item_width = item_width;
  this->item_height = item_height;
  this->item_padding = item_padding;
}

PaletteLayout::~PaletteLayout() throw() {}

void PaletteLayout::set_position (const Rectangle &location) throw() {
  Widget::set_position (location);
  for (int w = 0; w < get_item_count(); ++w) {
    Widget *widget = child_widgets[w];
    widget->set_position (calculate_widget_position (w));
  }
}

void PaletteLayout::consume (Widget *widget) throw() {
  Rectangle wid_pos = calculate_widget_position (get_item_count());

  // Ensure this layout is large enough to hold the consumed widget.
  if (wid_pos.y < 0) {
    Rectangle layout_pos = location;
    layout_pos.y -= -wid_pos.y;
    layout_pos.h += -wid_pos.y;
    set_position (layout_pos);
  }

  widget->set_position (wid_pos);

  Layout::consume (widget);
}

void PaletteLayout::unconsume (int index) throw() {
  assert (index >= 0 && index < (int)child_widgets.size());
  Widget *widget = child_widgets[index];
  Layout::unconsume (widget);
}
