/*
 * Editor.h
 *
 *  Created on: 2011-08-19
 *      Author: samuel
 */

#ifndef CONTROLLER_EDITOR_H
#define CONTROLLER_EDITOR_H

namespace Game {
  namespace Controllers {
    class Editor {
      friend class Manager;

    private:
      bool toggle_pressed;
      bool scroll_left_pressed, scroll_right_pressed,
           scroll_up_pressed, scroll_down_pressed;
      bool next_layer, previous_layer;
      bool next_brush, previous_brush;
      bool delete_pressed;

      // Only the ControllerManager should be able to construct and destruct this
      // type of controller.  Otherwise its state won't be updated.
      Editor();
      virtual ~Editor();

    public:
      bool is_toggle_pressed() const throw() { return toggle_pressed; }

      bool is_scroll_left_pressed() const throw() {
        return scroll_left_pressed;
      }
      bool is_scroll_right_pressed() const throw() {
        return scroll_right_pressed;
      }
      bool is_scroll_up_pressed() const throw() {
        return scroll_up_pressed;
      }
      bool is_scroll_down_pressed() const throw() {
        return scroll_down_pressed;
      }

      bool is_next_layer_pressed() const throw() { return next_layer; }
      bool is_previous_layer_pressed() const throw() { return previous_layer; }

      bool is_next_brush_pressed() const throw() { return next_brush; }
      bool is_previous_brush_pressed() const throw() { return previous_brush; }

      bool is_delete_pressed() const throw() { return delete_pressed; }
    };
  } // Controllers
}

#endif // CONTROLLER_EDITOR_H
