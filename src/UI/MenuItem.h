/*
 * MenuItem.h
 *
 *  Created on: 2012-01-05
 *      Author: samuel
 */

#ifndef MENUITEM_H
#define MENUITEM_H

#include <cstring> // for NULL
#include "Widget.h"

namespace Game {
  namespace UI {
    // Menu items can be highlighted (eg mouse hover) or selected (eg mouse
    // click).
    class MenuItem: public Widget {
    private:
      void (*select_callback) (void *select_callback_data);
      void *select_callback_data;

    protected:
      bool highlighted;
      bool selected;

    public:
      MenuItem() throw();
      virtual ~MenuItem() throw();

      void set_highlighted (bool highlighted) throw() {
        this->highlighted = highlighted;
      }

      void set_selected (bool selected) throw();

      void set_select_callback (
          void (*function) (void *select_callback_data) throw(),
          void *data = NULL) throw();

      virtual Widget *mouse_move (int x, int y) throw();
      virtual void mouse_primary_pressed (int x, int y) throw();
      virtual void mouse_exit() throw() { set_highlighted (false); }
    };
  } // namespace UI
} // namespace Game

#endif // MENUITEM_H
