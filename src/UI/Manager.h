/*
 * Manager.h
 *
 *  Created on: 2012-01-04
 *      Author: samuel
 */

#ifndef MANAGER_H
#define MANAGER_H

#include <list>
#include "Widget.h"
#include "FocusableWidget.h"

namespace Game {
  namespace UI {
    // Updates, displays, and handles user events related to widgets.
    class Manager {
    private:
      std::list<Widget*> widgets; // first is bottom-most, last is top-most

      Widget *hovered_widget;
      Widget *pressed_widget;
      FocusableWidget *focused_widget;

      int previous_mouse_x, previous_mouse_y;

    public:
      Manager() throw();
      virtual ~Manager() throw();

      // Take responsibility for a widget, including drawing and deletion.
      void consume (Widget *widget) throw();

      // Give up responsibility for a widget, including drawing and deletion.
      // If the widget isn't managed, nothing happens.
      void unconsume (Widget *widget) throw();

      // Disregard a widget somewhere in the hierarchy of widgets rooted in this
      // manager.  Use unconsume() instead if the widget in question was
      // directly consumed by this manager.
      void forget_widget (Widget *widget) throw();

      // Update all managed widgets.
      // tick is in ms
      void update (unsigned int tick) throw();

      // Draw all managed widgets.
      void draw() const throw();

      // Returns true only if the given pixel coordinate intersects a managed
      // widget.  x and y originate at the window's bottom-left.
      bool intersects (int x, int y) const throw();

      bool has_focus() const throw() { return focused_widget; }
    };
  } // namespace UI
} // namespace Game

#endif // MANAGER_H
