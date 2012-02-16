/*
 * Manager.h
 *
 *  Created on: 2011-08-20
 *      Author: samuel
 */

#ifndef CONTROLLER_MANAGER_H
#define CONTROLLER_MANAGER_H

#include <string>
#include "../Window.h"
#include "UI.h"
#include "Editor.h"

namespace Game {
  namespace Controllers {
    class Manager {
    private:
      static UI ui_controller;
      static Editor editor_controller;

      static bool escape_pressed;
      static bool app_closed;

      static bool terminal_toggle_pressed;
      static bool terminal_open;
      static int cursor_index_change;
      static std::string keyboard_buffer;

      Manager() throw();
      virtual ~Manager() throw();

    public:
      static UI *get_ui_controller() { return &ui_controller; }
      static Editor *get_editor_controller() { return &editor_controller; }

      // Updates the state of the managed controllers.
      static void update (const Window &window) throw();

      // General system controls.
      static bool is_escape_pressed() throw() { return escape_pressed; }
      static bool is_app_closed() throw() { return app_closed; }

      // Open the single terminal.
      static void open_terminal() throw();

      // Close the terminal and no longer accept keyboard input.
      static void close_terminal() throw();

      static bool is_terminal_toggled() throw() { return terminal_toggle_pressed; }
      static int get_cursor_index_change() throw();
      static std::string get_keyboard_buffer() throw();
    };
  } // Controllers
}

#endif // CONTROLLER_MANAGER_H
