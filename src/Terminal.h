/*
 * Terminal.h
 *
 *  Created on: 2012-01-22
 *      Author: samuel
 */

#ifndef TERMINAL_H
#define TERMINAL_H

#include "UI/Manager.h"
#include "UI/Label.h"
#include "UI/TextBox.h"
#include "World.h"

namespace Game {
  // A terminal for entering commands and seeing output.
  class Terminal {
  private:
    World &world;

    UI::Manager *ui_manager;
    UI::Label *output_label;
    UI::TextBox *input_textbox;
    bool visible;

    // Called by the terminal when enter is pressed.
    // static because the terminal callback that uses it can't call instance
    // methods
    static void on_enter_pressed (void *terminal) throw();

  public:
    Terminal (World &world) throw();
    virtual ~Terminal() throw();

    void set_visible (bool visible) throw();

    bool is_visible() const throw() { return visible; }

    // Puts a message into the terminal's output.
    void display_message (const std::string &message) {
      output_label->set_text (message);
    }

    // tick is in ms
    void update (unsigned int tick) throw() { ui_manager->update (tick); }

    void draw() const throw();
  }; // class Terminal
} // namespace Game

#endif // TERMINAL_H
