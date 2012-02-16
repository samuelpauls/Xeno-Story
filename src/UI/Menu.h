/*
 * Menu.h
 *
 *  Created on: 2011-09-18
 *      Author: samuel
 */

#ifndef MENU_H
#define MENU_H

#include "../Audio.h"
#include "Widget.h"
#include "PaletteLayout.h"
#include "MenuItem.h"

namespace Game {
  namespace UI {
    class Menu: public PaletteLayout {
    public:
      static const int NOT_SET = -1; // for highlighting and selecting

    private:
      Audio *change_sound; // due to highlight or select

      int highlight_index;
      int selection_index;

    public:
      Menu (int item_width, int item_height, int padding = 0) throw();
      virtual ~Menu() throw();

      // Returns the highest priority rectangle of this widget.  The rectangle
      // is relative to the parent widget or, if no parent, the window.  Origin
      // at the bottom-left.
      virtual Rectangle get_priority_rectangle() const throw();

      // Select the item underneath the given x and y pixel relative to the
      // window's origin.
      // True is returned if a menu item is selected due to this method call or
      // false otherwise.
      bool select_by_pixel (int window_x, int window_y) throw();

      void increment_selection() throw();
      void decrement_selection() throw();

      void increment_highlight() throw();
      void decrement_highlight() throw();
      void select_highlighted() throw();

      int get_selection_index() const throw() { return selection_index; }

      // Select an item by its index.  True is returned if the call resulted in a
      // changed selection or false otherwise.
      bool select_by_index (int index) throw();

      MenuItem *get_highlighted_widget() const throw();
      MenuItem *get_selected_widget() const throw();

      virtual Widget *mouse_move (int x, int y) throw();
      virtual void mouse_primary_pressed (int x, int y) throw();
      virtual void mouse_exit() throw();
    }; // Menu
  } // namespace Menu
} // namespace Game

#endif // MENU_H
