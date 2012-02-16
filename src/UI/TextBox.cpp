/*
 * TextBox.cpp
 *
 *  Created on: 2011-10-13
 *      Author: samuel
 */

#include <cmath> // for std::abs
#include <climits> // for INT_MAX
#include <GL/gl.h>
#include "../Rectangle.h"
#include "../Clipper.h"
#include "../Controllers/Manager.h"
#include "TextBox.h"

using Game::UI::TextBox;

Game::Font TextBox::font (10);

TextBox::TextBox() throw() {
  cursor_index = 0;
  cursor_visible = false;

  enter_callback_function = NULL;
  enter_callback_data = NULL;
}

TextBox::~TextBox() throw() {
  set_focus (false);
}

void TextBox::set_enter_callback (void (*function) (void *data) throw(),
                                   void *data) throw() {
  enter_callback_function = function;
  enter_callback_data = data;
}

void TextBox::set_text (const std::string &text) throw() {
  this->text = text;
  cursor_index = text.length();
}

void TextBox::set_focus (bool focus) throw() {
  FocusableWidget::set_focus (focus);
  if (focus) {
    Controllers::Manager::open_terminal();
  } else {
    Controllers::Manager::close_terminal();
    enter_callback_function (enter_callback_data);
  }
}

void TextBox::mouse_primary_pressed (int x, int y) throw() {
  int dist = INT_MAX; // horizontal pixels between potential cursor position and
                      // mouse pointer
  for (unsigned int t = 0; t <= text.length(); ++t) {
    int new_dist = std::abs (
        x - location.x - TEXT_PADDING - font.get_width (text.substr(0, t)));
    if (new_dist < dist) {
      cursor_index = t;
    }
    dist = new_dist;
  }
}

void TextBox::update (unsigned int tick) throw() {
  if (focus) {
    std::string buffer = Controllers::Manager::get_keyboard_buffer();
    if (!buffer.empty()) {
      // Insert the buffer into the command.
      text = text.substr (0, cursor_index) + buffer +
          text.substr (cursor_index);
      cursor_index += buffer.size();

      // Process backspaces.
      for (size_t c = 0; c < text.size(); ++c) {
        if (text[c] == '\b') {
          if (c == 0) {
            // Remove the first character which is a backspace.
            text = text.substr (1);
          } else {
            // Remove the character before the backspace in addition to the
            // backspace.
            std::string after_removal;
            if (c < text.size() - 1) {
              after_removal = text.substr (c + 1);
            }
            text = text.substr (0, c - 1) + after_removal;
          }
        }
      }

      // Execute the command upon pressing enter.
      if (text[text.size() - 1] == '\n') {
        text = text.substr (0, text.size() - 1); // remove '\n'
        enter_callback_function (enter_callback_data);
      }
    } // if keyboard buffer

    // Move the cursor.
    cursor_index += Controllers::Manager::get_cursor_index_change();
    if (cursor_index < 0) {
      cursor_index = 0;
    } else if (cursor_index > (int)text.size()) {
      cursor_index = text.size();
    }

    // Flash the cursor.
    static const int CURSOR_RATE = 500; // ms that the cursor is on or off
    cursor_visible = (tick / CURSOR_RATE) % 2;
  } else { // if no focus
    cursor_visible = false;
  }
}

void TextBox::draw() const throw() {
  // Translate to this terminal's relative location.
  glPushMatrix();
  glTranslatef (location.x, location.y, 0.0F);

  // Draw the background.
  glColor4f (1.0F, 1.0F, 1.0F, 0.8F); // 80% white
  glBegin (GL_QUADS);
    glVertex2i (0, 0);
    glVertex2i (0, location.h);
    glVertex2i (location.w, location.h);
    glVertex2i (location.w, 0);
  glEnd();

  glColor3f (0.0F, 0.0F, 0.0F); // black

  // Draw the text.
  Clipper::push (get_position_in_window());
  glTranslatef (TEXT_PADDING, TEXT_PADDING, 0.0F);
  font.draw (text);
  Clipper::pop();

  // Draw the cursor.
  if (cursor_visible) {
    int x = font.get_width (text.substr (0, cursor_index));
    glBegin (GL_LINES);
      glVertex2i (x, 0);
      glVertex2i (x, font.get_line_height());
    glEnd();
  }

  glPopMatrix();
}
