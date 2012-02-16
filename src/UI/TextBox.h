/*
 * TextBox.h
 *
 *  Created on: 2011-10-13
 *      Author: samuel
 */

#ifndef TEXTBOX_H
#define TEXTBOX_H

#include <string>
#include "../Font.h"
#include "FocusableWidget.h"

namespace Game {
  namespace UI {
    // A single line of editable text.
    class TextBox: public FocusableWidget {
    private:
      bool cursor_visible;
      int cursor_index;

      static Font font;
      static const int TEXT_PADDING = 4;
      std::string text;

      void (*enter_callback_function) (void *data) throw();
      void *enter_callback_data;

    public:
      TextBox() throw();
      virtual ~TextBox() throw();

      // Set the function to call upon pressing enter.
      void set_enter_callback (void (*function) (void *data) throw(),
                               void *data = NULL) throw();

      void set_text (const std::string &text) throw();
      std::string get_text() const throw() { return text; }

      virtual void set_focus (bool focus) throw();

      virtual void mouse_primary_pressed (int x, int y) throw();

      virtual void update (unsigned int tick) throw();

      virtual void draw() const throw();
    }; // class TextBox
  } // namespace UI
} // namespace Game

#endif // TEXTBOX_H
