/*
 * Frame.cpp
 *
 *  Created on: 2011-12-21
 *      Author: samuel
 */

#include <cassert>
#include <GL/gl.h>
#include "../Clipper.h"
#include "Frame.h"

using Game::UI::Frame;

Game::Font Frame::title_font (10);

Frame::Frame (const std::string &title) throw() {
  this->title = title;
  dragging = false;
  resizing_left = false;
  resizing_right = false;
  resizing_bottom = false;
}

Frame::~Frame() throw() {}

void Frame::set_position (const Rectangle &location) throw() {
  Widget::set_position (location);
  background.set_size (location.w, location.h);
  if (get_child_count() > 0) {
    get_widget(0)->set_position (get_child_position());
  }

  title_rectangle.x = 0;
  title_rectangle.y = location.h - title_font.get_line_height();
  title_rectangle.w = location.w;
  title_rectangle.h = title_font.get_line_height();
}

Game::Rectangle Frame::get_child_position() {
  Rectangle widget_rect;
  widget_rect.x = PADDING;
  widget_rect.y = PADDING;
  widget_rect.w = location.w - PADDING * 2;
  widget_rect.h = location.h - TOP_PADDING - PADDING;
  return widget_rect;
}

void Frame::consume (Widget *widget) throw() {
  assert (get_child_count() == 0);
  widget->set_position (get_child_position());
  Layout::consume (widget);
}

void Frame::unconsume() throw() {
  Layout::unconsume (get_widget (0));
}

void Frame::draw() const throw() {
  // Translate to the frame's relative location.
  glPushMatrix();
  glTranslatef (location.x, location.y, 0.0F);

  background.draw();

  // Draw the title.
  Clipper::push (get_position_in_window()); // keep title within frame
  GLfloat title_x = title_rectangle.x +
      (title_rectangle.w - title_font.get_width (title)) / 2.0F;
  GLfloat title_y = title_rectangle.y +
      (title_rectangle.h - title_font.get_line_height()) / 2.0F;
  glTranslatef (title_x, title_y, 0.0F);
  glColor3ub (106, 74, 252);
  title_font.draw (title);
  Clipper::pop();

  glPopMatrix();

  Layout::draw();
}

Game::UI::Widget *Frame::mouse_move (int x, int y) throw() {
  if (dragging) {
    location.x = x - drag_x;
    location.y = y - drag_y;
  } else if (resizing_left || resizing_right || resizing_bottom) {
    Rectangle new_location = location;
    if (resizing_left) {
      new_location.x = x;
      new_location.w += location.x - x;

      // Ensure this frame's width is at least enough for the borders.
      if (new_location.w < 2 * PADDING) {
        new_location.x = location.get_right() - 2 * PADDING;
        new_location.w = 2 * PADDING;
      }
    } else if (resizing_right) {
      new_location.w += x - location.get_right();

      // Ensure this frame's width is at least enough for the borders.
      if (new_location.w < 2 * PADDING) {
        new_location.w = 2 * PADDING;
      }
    }
    if (resizing_bottom) {
      new_location.y = y;
      new_location.h += location.y - y;

      // Ensure this frame's height is at least enough for the borders.
      if (new_location.h < PADDING + TOP_PADDING) {
        new_location.y = location.get_top() - TOP_PADDING - PADDING;
        new_location.h = PADDING + TOP_PADDING;
      }
    }
    set_position (new_location);
  } else {
    return Layout::mouse_move (x, y);
  }
  return this;
}

void Frame::mouse_primary_pressed (int x, int y) throw() {
  int x_relative_to_frame = x - location.x;
  int y_relative_to_frame = y - location.y;

  if (get_child_position().intersects (x_relative_to_frame,
                                       y_relative_to_frame)) {
    Layout::mouse_primary_pressed (x, y);
  } else if (title_rectangle.intersects (x_relative_to_frame,
                                         y_relative_to_frame)) {
    // Drag the frame.
    drag_x = x_relative_to_frame;
    drag_y = y_relative_to_frame;
    dragging = true;
  } else { // If the mouse is clicked down on this frame's border...
    if (x_relative_to_frame < PADDING) {
      resizing_left = true;
    } else if (x_relative_to_frame > PADDING + get_child_position().w) {
      resizing_right = true;
    }
    if (y_relative_to_frame < PADDING) {
      resizing_bottom = true;
    }
  }
}

void Frame::mouse_primary_released (int x, int y) throw() {
  Layout::mouse_primary_released (x, y);
  dragging = false;
  resizing_left = false;
  resizing_right = false;
  resizing_bottom = false;
}
