/*
 * Layout.h
 *
 *  Created on: 2012-01-14
 *      Author: samuel
 */

#ifndef LAYOUT_H
#define LAYOUT_H

#include <vector>
#include "Widget.h"

namespace Game {
  namespace UI {
    // A layout of widgets.  A widget potentially overlaps another, but is
    // brought to the top when activated.
    class Layout: public Widget {
    private:
      Widget *pressed_widget;

    protected:
      std::vector<Widget*> child_widgets;

      // Take responsibility for a widget including passing events, drawing,
      // deleting and so on.
      void consume (Widget *widget) throw();

      // No longer take a responsibility for a previously consumed widget.
      // Remember to delete the unconsumed widget yourself when done with it.
      void unconsume (Widget *widget) throw();

    public:
      Layout() throw();
      virtual ~Layout() throw();

      int get_child_count() const throw() { return child_widgets.size(); }

      Widget *get_widget (int index) const throw();

      virtual Widget *mouse_move (int x, int y) throw();
      virtual void mouse_primary_pressed (int x, int y) throw();
      virtual void mouse_primary_released (int x, int y) throw();
      virtual void mouse_exit() throw();

      virtual void update (unsigned int tick) throw();

      virtual void draw() const throw();
    };
  } // namespace UI
} // namespace Game

#endif // LAYOUT_H
