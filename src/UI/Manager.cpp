/*
 * Manager.cpp
 *
 *  Created on: 2012-01-04
 *      Author: samuel
 */

#include <GL/gl.h>
#include "../Controllers/Manager.h"
#include "../Controllers/UI.h"
#include "Layout.h"
#include "Manager.h"

using Game::UI::Manager;

Manager::Manager() throw() {
  // to be changed on first update
  previous_mouse_x = -1;
  previous_mouse_y = -1;

  pressed_widget = NULL;
  focused_widget = NULL;
  hovered_widget = NULL;
}

Manager::~Manager() throw() {
  // Delete the managed widgets.
  for (std::list<Widget*>::const_iterator w = widgets.begin();
       w != widgets.end();
       ++w) {
    delete *w;
  }
}

void Manager::consume (Widget *widget) throw() {
  widget->set_manager (this);
  widgets.push_back (widget);
}

void Manager::unconsume (Widget *widget) throw() {
  widgets.remove (widget);
  widget->set_manager (NULL);
  forget_widget (widget);
}

void Manager::forget_widget (Widget *widget) throw() {
  if (hovered_widget == widget) {
    hovered_widget = NULL;
  }
  if (pressed_widget == widget) {
    pressed_widget = NULL;
  }
  if (focused_widget == widget) {
    focused_widget = NULL;
  }

  // Recursively forget about subwidgets.
  Layout *layout = dynamic_cast <Layout*> (widget);
  if (layout) {
    for (int w = 0; w < layout->get_child_count(); ++w) {
      forget_widget (layout->get_widget (w));
    }
  }
}

void Manager::update (unsigned int tick) throw() {
  // Process mouse events.
  static Controllers::UI *uic = Controllers::Manager::get_ui_controller();

  int mouse_x = uic->get_x();
  int mouse_y = uic->get_y();

  // If the mouse moved...
  if (mouse_x != previous_mouse_x || mouse_y != previous_mouse_y) {
    if (pressed_widget) {
      // Send the mouse move event to the already pressed widget.
      pressed_widget->mouse_move (mouse_x, mouse_y);
    } else {
      // Send a mouse exit event to the previously hovered widget if the mouse
      // is no longer hovering over it.
      if (hovered_widget && !hovered_widget->intersects (mouse_x, mouse_y)) {
        hovered_widget->mouse_exit();
        hovered_widget = NULL;
      }

      for (std::list<Widget*>::const_reverse_iterator w = widgets.rbegin();
           w != widgets.rend();
           ++w) {
        Widget *widget = *w;
        if (widget->get_position().intersects (mouse_x, mouse_y)) {
          // Send the mouse move event to the widget under the mouse.
          Widget *new_hovered_widget = widget->mouse_move (mouse_x, mouse_y);

          // Send a mouse exit event to the previously hovered widget if the
          // hovered widget changes.
          if (hovered_widget && hovered_widget != new_hovered_widget) {
            hovered_widget->mouse_exit();
            hovered_widget = NULL;
          }

          hovered_widget = new_hovered_widget;
          break;
        }
      }
    }

    previous_mouse_x = mouse_x;
    previous_mouse_y = mouse_y;
  }

  // If the mouse's primary button is down...
  if (uic->is_primary_down()) {
    if (!pressed_widget) {
      for (std::list<Widget*>::const_reverse_iterator w = widgets.rbegin();
           w != widgets.rend();
           ++w) {
        Widget *widget = *w;
        if (widget->get_position().intersects (mouse_x, mouse_y)) {
          // Bring widget to the front.
          widgets.remove (widget);
          widgets.push_back (widget);

          widget->mouse_primary_pressed (mouse_x, mouse_y);
          pressed_widget = widget;

          // Change focus.
          if (focused_widget) {
            focused_widget->set_focus (false);
            focused_widget = NULL;
          }
          FocusableWidget *fw = dynamic_cast <FocusableWidget*> (hovered_widget);
          if (fw) {
            focused_widget = fw;
            focused_widget->set_focus (true);
          }

          break;
        }
      }
    }
  } else { // If mouse's primary button is up...
    if (pressed_widget) {
      pressed_widget->mouse_primary_released (mouse_x, mouse_y);
      pressed_widget = NULL;
    }
  }

  // Update managed widgets.
  for (std::list<Widget*>::const_iterator w = widgets.begin();
       w != widgets.end();
       ++w) {
    (*w)->update (tick);
  }
}

void Manager::draw() const throw() {
  // Translate to the UI origin.
  glPushMatrix();
  glLoadIdentity();

  // Draw the managed widgets.
  for (std::list<Widget*>::const_iterator w = widgets.begin();
       w != widgets.end();
       ++w) {
    (*w)->draw();
  }

  glPopMatrix();
}

bool Manager::intersects (int x, int y) const throw() {
  for (std::list<Widget*>::const_iterator w = widgets.begin();
       w != widgets.end();
       ++w) {
    if ((*w)->get_position().intersects (x, y)) {
      return true;
    }
  }
  return false;
}
