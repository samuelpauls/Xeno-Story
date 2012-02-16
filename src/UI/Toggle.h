/*
 * Toggle.h
 *
 *  Created on: 2012-01-09
 *      Author: samuel
 */

#ifndef TOGGLE_H
#define TOGGLE_H

#include "../Animation.h"
#include "Widget.h"

namespace Game {
  namespace UI {
    // Toggle's something on or off.
    class Toggle: public Widget {
    private:
      bool value;
      bool tentative_value; // value for once sliding ends

      void (*value_changed_callback) (void *value_changed_callback_data);
      void *value_changed_callback_data;

      Animation *background;

      Animation *slider;
      Animation *slider_highlighted;
      Animation *current_slider;
      Rectangle slider_rectangle; // relative to toggle's bottom-left
      bool dragging_slider;

    public:
      Toggle (bool state = false) throw();
      virtual ~Toggle() throw();

      void set_value (bool value) throw();

      bool get_value() const throw() { return value; }

      void set_value_changed_callback (void (*function)(void *data),
                                       void *data = NULL) throw();

      virtual void update (unsigned int tick) throw();

      virtual void draw() const throw();

      virtual Widget *mouse_move (int x, int y) throw();

      virtual void mouse_primary_pressed (int x, int y) throw();

      virtual void mouse_primary_released (int x, int y) throw();

      virtual void mouse_exit() throw();
    };
  } // namespace UI
} // namespace Game

#endif // TOGGLE_H
