/*
 * Widget.h
 *
 *  Created on: 2011-12-28
 *      Author: samuel
 */

#ifndef WIDGET_H
#define WIDGET_H

#include "../Rectangle.h"

namespace Game {
  namespace UI {
    class Manager;

    // A GUI widget such as a label or button.
    class Widget {
    protected:
      Manager *manager;
      Widget *parent;

      Rectangle location; // origin at bottom-left of window or, if embedded,
                          // parent widget

      // Retrieves the bottom-left position of this widget within the window.
      // x and y must be set to 0 before calling this method from outside this
      // method.  Should only be called by get_position_in_window().
      virtual void get_position_in_window (int &x, int &y) const throw();

      // Retrieves the position of this widget relative to the window's origin.
      Rectangle get_position_in_window() const throw();

    public:
      Widget() throw();
      virtual ~Widget() throw();

      void set_manager (Manager *manager) throw() { this->manager = manager; }

      Manager *get_manager() const throw() { return manager; }

      void set_parent (Widget *widget) throw() { parent = widget; }

      Widget *get_parent() const throw() { return parent; }

      Rectangle get_position() const throw() { return location; }

      // Set the widget's position relative to the bottom-left of the window or,
      // if embedded, parent widget.
      virtual void set_position (const Rectangle &location) throw() {
        this->location = location;
      }

      // Returns the highest priority rectangle of this widget.  The rectangle
      // is relative to the parent widget or, if no parent, the window.  Origin
      // at the bottom-left.
      virtual Rectangle get_priority_rectangle() const throw() {
        return location;
      }

      // Returns true if this widget intersects the given window pixel,
      // originating in the bottom left.
      bool intersects (int window_x, int window_y) const throw();

      // Called every frame, even if no user events occur.
      // tick is in ms
      virtual void update (unsigned int tick) throw() {}

      // Draw this widget relative with consideration for its position.
      virtual void draw() const throw() = 0;

      // Called when the mouse hovers over this widget's location or the mouse
      // is pressing this widget, even if it has since left this widget's
      // location.
      // x and y are pixels from the widget's location's origin.
      // Returns the embedded most widget that the mouse hovers over.
      virtual Widget *mouse_move (int x, int y) throw() { return this; }

      // Called when the mouse's primary button clicks down over this widget's
      // location.
      // x and y are pixels from the widget's location's origin.
      virtual void mouse_primary_pressed (int x, int y) throw() {}

      // Called when the mouse's primary button raises up over this widget's
      // location.
      // x and y are pixels from the widget's location's origin.
      virtual void mouse_primary_released (int x, int y) throw() {}

      // Called when the mouse leaves this widget's location.
      virtual void mouse_exit() throw() {}
    };
  } // namespace UI
} // namespace Game

#endif // WIDGET_H
