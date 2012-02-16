/*
 * GridLayout.cpp
 *
 *  Created on: 2012-01-11
 *      Author: samuel
 */

#include <algorithm> // for std::min
#include <cstring> // for NULL
#include <GL/gl.h>
#include "GridLayout.h"

using Game::UI::GridLayout;

void GridLayout::calculate_widget_size() throw() {
  // Calculate the widget width.
  item_width = (location.w - item_padding) /
               widget_grid->get_column_count() - item_padding;
  if (maximum_width != UNLIMITED) {
    item_width = std::min (item_width, maximum_width);
  }

  // Calculate the widget height.
  item_height = (location.h - item_padding) / widget_grid->get_row_count()
                - item_padding;
  if (maximum_height != UNLIMITED) {
    item_height = std::min (item_height, maximum_height);
  }
}

void GridLayout::position_widget (int column, int row, Widget *widget) throw() {
  Rectangle wid_pos;
  wid_pos.x = item_padding + column * (item_width + item_padding);
  wid_pos.y = location.h - (row + 1) * (item_height + item_padding);
  wid_pos.w = item_width;
  wid_pos.h = item_height;
  widget->set_position (wid_pos);
}

GridLayout::GridLayout (int columns, int rows,
                        int item_padding,
                        int maximum_width, int maximum_height) throw() {
  widget_grid = new Layout2D<Widget*> (columns, rows, NULL);
  this->item_padding = item_padding;
  this->maximum_width = maximum_width;
  this->maximum_height = maximum_height;
  calculate_widget_size();
}

GridLayout::~GridLayout() throw() {
  delete widget_grid;
}

void GridLayout::set_position (const Rectangle &location) throw() {
  Widget::set_position (location);
  calculate_widget_size();

  // Set the positions of all consumed widgets.
  for (int c = 0; c < widget_grid->get_column_count(); ++c) {
    for (int r = 0; r < widget_grid->get_row_count(); ++r) {
      Widget *widget = widget_grid->get (c, r);
      if (widget) {
        position_widget (c, r, widget);
      }
    }
  }
}

void GridLayout::consume (int column, int row, Widget *widget) throw() {
  position_widget (column, row, widget);
  widget_grid->set (column, row, widget);
  Layout::consume (widget);
}

void GridLayout::unconsume (int column, int row) throw() {
  Widget *w = widget_grid->get (column, row);
  assert (w);
  widget_grid->set (column, row, NULL);
  Layout::unconsume (w);
}
