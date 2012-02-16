/*
 * ScrollableContainer.cpp
 *
 *  Created on: 2012-01-03
 *      Author: samuel
 */

#include <cassert>
#include <cstring> // for NULL
#include "../Rectangle.h"
#include "../Clipper.h"
#include "ScrollableContainer.h"

using Game::UI::ScrollableContainer;

ScrollableContainer::ScrollableContainer() throw() {}

ScrollableContainer::~ScrollableContainer() throw() {}

void ScrollableContainer::set_position (const Rectangle &location) throw() {
  Widget::set_position (location);
  if (get_child_count() > 0) {
    get_widget(0)->set_position (location);
  }
}

void ScrollableContainer::consume (Widget *widget) throw() {
  assert (get_child_count() == 0);
  widget->set_position (location);
  Layout::consume (widget);
}

void ScrollableContainer::unconsume() throw() {
  assert (get_child_count() > 0);
  Layout::unconsume (get_widget(0));
}

void ScrollableContainer::update (unsigned int tick) throw() {
  if (get_child_count() > 0) {
    Widget &child_widget = *get_widget(0);
    Rectangle priority_rect = child_widget.get_priority_rectangle();
    Rectangle child_pos = child_widget.get_position();
    bool child_pos_changed = false;

    // Set the child's horizontal position such that the priority rectangle
    // remains visible.
    if (priority_rect.x < 0) {
      child_pos.x += -priority_rect.x;
      child_pos_changed = true;
    } else if (priority_rect.get_right() > location.w) {
      child_pos.x -= priority_rect.get_right() - location.w;
      child_pos_changed = true;
    }

    // Set the child's vertical position such that the priority rectangle
    // remains visible.
    if (priority_rect.y < 0) {
      child_pos.y += -priority_rect.y;
      child_pos_changed = true;
    } else if (priority_rect.get_top() > location.h) {
      child_pos.y -= priority_rect.get_top() - location.h;
      child_pos_changed = true;
    }

    if (child_pos_changed) {
      child_widget.set_position (child_pos);
    }

    Layout::update (tick);
  }
}

void ScrollableContainer::draw() const throw() {
  if (get_child_count() > 0) {
    // Clip the part of the widget that may be drawn outside of this container.
    Clipper::push (get_position_in_window());

    Layout::draw();

    Clipper::pop();
  }
}
