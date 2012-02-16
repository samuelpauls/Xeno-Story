/*
 * UI.h
 *
 *  Created on: 2012-01-04
 *      Author: samuel
 */

#ifndef CONTROLLERS_UI_H
#define CONTROLLERS_UI_H

namespace Game {
  namespace Controllers {
    class UI {
      friend class Manager;

    private:
      bool primary_down, secondary_down;
      int x, y;

      // Only the ControllerManager should be able to construct and destruct this
      // type of controller.  Otherwise its state won't be updated.
      UI() throw();
      virtual ~UI() throw();

    public:
      // Get the mouse position.  The origin is at the Window's bottom-left and
      // y increases on it's way to the top of the window.
      int get_x() const throw() { return x; }
      int get_y() const throw() { return y; }

      // Returns true if the primary mouse button is currently pressed down.
      bool is_primary_down() const throw() { return primary_down; }

      // Returns true if the secondary mouse button is currently pressed down.
      bool is_secondary_down() const throw() { return secondary_down; }
    };
  } // namespace Controllers
} // namespace Game

#endif // CONTROLLERS_UI_H
